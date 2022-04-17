#include <Windows.h>
#include <atomic>
#include <cassert>
#include "JobQueue.h"
#include <mutex>
#include <unordered_map>

namespace DuckLib
{
Job::Job()
	: jobCounter(nullptr)
, jobFunction(nullptr)
, jobData(nullptr)
{}

Job::Job(void (*jobFunction)(void*), void* jobData)
	: jobCounter(nullptr)
, jobFunction(jobFunction)
, jobData(jobData)
{}

namespace Internal
{
std::atomic<bool> runWorkers;
thread_local Fiber workerThreadFiber {};
thread_local bool isWorkerThread { false };
thread_local Fiber* currentFiber {};

void SwitchFiber(Fiber* fiber)
{
#ifdef _WIN32
	SwitchToFiber(fiber->osFiber);
#endif
}

void SwitchToWorker()
{
	SwitchFiber(&workerThreadFiber);
}

void InitWorkerThread()
{
	isWorkerThread = true;
#ifdef _WIN32
	workerThreadFiber.osFiber = ConvertThreadToFiber(&workerThreadFiber);
#endif
}

uint32_t _stdcall WorkerThreadJob(void* data)
{
	JobQueue::WorkerThreadData* workerThreadData = (JobQueue::WorkerThreadData*)data;
	std::atomic<bool>& runFlag = workerThreadData->runFlag;
	std::atomic<bool>& startFlag = workerThreadData->startFlag;
	JobQueue* jobQueue = workerThreadData->jobQueue;

	InitWorkerThread();

	while (!startFlag.load())
		YieldThread(10);

	while (runFlag.load())
	{
		Fiber* jobFiber = jobQueue->GetReadyJobAndFiber();

		if (!jobFiber)
		{
			YieldThread(10);
			continue;
		}

		if (!jobFiber->currentJob.jobFunction)
			throw std::exception("Tried to start a fiber with a nullptr job");
		
		currentFiber = jobFiber;
		SwitchFiber(currentFiber);
		currentFiber = nullptr;
		jobQueue->ReturnFiberIfJobCompleted(jobFiber);
	}

	return 0;
}

void _stdcall FiberJobWrapper(void* data)
{
	Fiber* fiberData = (Fiber*)data;

	while (true)
	{
		Job job = fiberData->currentJob;

		job.jobFunction(job.jobData);
		job.jobCounter->Decrement();
		fiberData->currentJob = {};
		SwitchToWorker();
	}
}
}

void JobCounter::Reset()
{
	waitingJobFiber = nullptr;
}

void JobCounter::Decrement()
{
	if (--counter == 0 && waitingJobFiber)
		jobQueue->FinalizeCompletedJobCounter(this);
}

JobQueue::JobQueue(uint32_t size, uint32_t numFibers, uint32_t numWorkers)
	: alloc(DefAlloc())
{
	this->numWorkers = numWorkers == MATCH_NUM_LOGICAL_CORES ? GetNumLogicalCores() : numWorkers;
	queueSize = size;

	uintptr_t* initPtrArrayBuffer = DL_NEW_ARRAY(alloc, uintptr_t, max(size, numFibers));

	SetupCounters(size, initPtrArrayBuffer);
	SetupFibers(numFibers, initPtrArrayBuffer);
	SetupJobStorage(size);
	SetupWorkers(this->numWorkers);

	DL_DELETE_ARRAY(alloc, initPtrArrayBuffer);

	workerThreadData.startFlag.store(true);
}

JobQueue::~JobQueue()
{
	TearDownWorkers();
	TearDownJobStorage();
	TearDownFibers();
	TearDownCounters();
}

JobCounter* JobQueue::Push(Job* jobs, uint32_t numJobs)
{
	JobCounter* jobCounter;

	if (!counterQueue->TryPop(&jobCounter))
		throw std::exception("Failed to acquire job counter");

	for (uint32_t i = 0; i < numJobs; ++i)
		jobs[i].jobCounter = jobCounter;

	uint32_t jobsQueued = jobQueue->TryPush(jobs, numJobs);

	if (jobsQueued != numJobs)
		throw std::exception("Failed to push jobs to queue");

	jobCounter->counter.store(jobsQueued);

	return jobCounter;
}

void JobQueue::WaitForCounter(JobCounter* counter)
{
	if (Internal::isWorkerThread)
	{
		counter->waitingJobFiber = Internal::currentFiber;
		std::atomic_thread_fence(std::memory_order_release);
		Internal::SwitchToWorker();
	}
	else
		WaitIdle(counter);
}

Internal::Fiber* JobQueue::GetReadyJobAndFiber()
{
	Internal::Fiber* readyPausedJobFiber;

	if (readyPausedJobFiberQueue->TryPop(&readyPausedJobFiber))
		return readyPausedJobFiber;

	Job newJob;
	Internal::Fiber* newJobFiber;

	if (jobQueue->TryPop(&newJob))
	{
		if (!fiberQueue->TryPop(&newJobFiber))
			throw std::exception("Failed to acquire fiber for new job");
		
		newJobFiber->currentJob = newJob;
		return newJobFiber;
	}

	return nullptr;
}

void JobQueue::ReturnFiberIfJobCompleted(Internal::Fiber* completedFiber)
{
	// jobFunction == nullptr -> completed, otherwise paused
	if (completedFiber->currentJob.jobFunction)
		return;
	
	if (!fiberQueue->TryPush(completedFiber))
		throw std::exception("Failed to push used fiber back on fiber queue. Wtf?");
}

void JobQueue::FinalizeCompletedJobCounter(JobCounter* counter)
{
	if (!readyPausedJobFiberQueue->TryPush(counter->waitingJobFiber))
		throw std::exception("Failed to push job onto job queue");

	counter->Reset();

	if (!counterQueue->TryPush(counter))
		throw std::exception("Failed to push job counter back on job counter queue");
}

Internal::Fiber JobQueue::CreateFiber(void* fiberData)
{
	Internal::Fiber fiber;

	fiber.currentJob = {};
#ifdef _WIN32
	fiber.osFiber = ::CreateFiber((SIZE_T)Internal::Fiber::DEFAULT_STACK_SIZE,
		(LPFIBER_START_ROUTINE)&Internal::FiberJobWrapper,
		fiberData);
#endif

	return fiber;
}

void JobQueue::DeleteFiber(Internal::Fiber* fiber)
{
#ifdef _WIN32
	::DeleteFiber(fiber->osFiber);
#endif
}

uint32_t JobQueue::GetNumLogicalCores() const
{
#ifdef _WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
#endif
}

void JobQueue::WaitIdle(JobCounter* counter)
{
	while (counter->counter.load() != 0)
		Sleep(5);
}

void JobQueue::SetupCounters(uint32_t numCounters, uintptr_t* initPtrArrayBuffer)
{
	this->numCounters = numCounters;
	counters = DL_NEW_ARRAY(DefAlloc(), JobCounter, numCounters);

	for (uint32_t i = 0; i < numCounters; ++i)
	{
		counters[i].jobQueue = this;
		initPtrArrayBuffer[i] = (uintptr_t)&counters[i];
	}

	counterQueue = New<ConcurrentQueue<JobCounter*>>(numCounters, (JobCounter**)initPtrArrayBuffer, numCounters);
}

void JobQueue::SetupFibers(uint32_t numFibers, uintptr_t* initPtrArrayBuffer)
{
	this->numFibers = numFibers;
	fibers = DL_NEW_ARRAY(alloc, Internal::Fiber, numFibers);

	for (uint32_t i = 0; i < numFibers; ++i)
	{
		fibers[i] = CreateFiber(&fibers[i]);
		initPtrArrayBuffer[i] = (uintptr_t)&fibers[i];
	}

	fiberQueue = DL_NEW(alloc,
		ConcurrentQueue<Internal::Fiber*>,
		numFibers,
		(Internal::Fiber**)initPtrArrayBuffer,
		numFibers);
}

void JobQueue::SetupJobStorage(uint32_t size)
{
	readyPausedJobFiberQueue = DL_NEW(alloc, ConcurrentQueue<Internal::Fiber*>, size);
	jobQueue = DL_NEW(alloc, ConcurrentQueue<Job>, size);
}

void JobQueue::SetupWorkers(uint32_t numWorkers)
{
	workerThreads = DL_NEW_ARRAY(alloc, Thread*, numWorkers);

	workerThreadData.jobQueue = this;
	workerThreadData.runFlag.store(true);
	workerThreadData.startFlag.store(false);

	for (uint32_t i = 0; i < numWorkers; ++i)
		workerThreads[i] = DL_NEW(alloc,
			Thread,
			Internal::WorkerThreadJob,
			&workerThreadData);
}

void JobQueue::TearDownWorkers()
{
	workerThreadData.runFlag.store(false);

	for (uint32_t i = 0; i < numWorkers; ++i)
	{
		workerThreads[i]->Join();
		DL_DELETE(DefAlloc(), workerThreads[i]);
	}

	DL_DELETE_ARRAY(DefAlloc(), workerThreads);
}

void JobQueue::TearDownJobStorage()
{
	// TODO: Consider checking if all jobs have been completed before tearing down? Or quick exit?
	DL_DELETE(DefAlloc(), jobQueue);
	DL_DELETE(DefAlloc(), readyPausedJobFiberQueue);
}

void JobQueue::TearDownFibers()
{
	for (uint32_t i = 0; i < numFibers; ++i)
		DeleteFiber(&fibers[i]);

	DL_DELETE(DefAlloc(), fiberQueue);
	DL_DELETE_ARRAY(DefAlloc(), fibers);
}

void JobQueue::TearDownCounters()
{
	DL_DELETE(DefAlloc(), counterQueue);
	DL_DELETE_ARRAY(DefAlloc(), counters);
}
}
