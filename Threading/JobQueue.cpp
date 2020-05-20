#include <Windows.h>
#include <atomic>
#include <cassert>
#include "JobQueue.h"

namespace DuckLib
{
Job::Job()
	: jobFunction(nullptr)
, jobData(nullptr)
, jobCounter(nullptr)
{}

Job::Job(void (*jobFunction)(void*), void* jobData)
	: jobFunction(jobFunction)
, jobData(jobData)
, jobCounter(nullptr)
{}

namespace Internal
{
std::atomic<bool> runWorkers;
thread_local Fiber workerThreadFiber {};
thread_local bool isWorkerThread { false };
thread_local Job currentJob {};

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
	JobQueue* jobDoer = workerThreadData->jobDoer;

	InitWorkerThread();

	while (!startFlag.load())
		YieldThread(10);

	while (runFlag.load())
	{
		Fiber* jobFiber;

		if (!jobDoer->GetReadyJob(&currentJob))
		{
			YieldThread(10);
			continue;
		}

		// TODO: Decide what to do if popping or pushing fails
		jobDoer->fiberQueue->TryPop(&jobFiber);
		jobFiber->currentJob = &currentJob;
		SwitchFiber(jobFiber);
		jobDoer->fiberQueue->TryPush(jobFiber);
		currentJob = {};
	}

	return 0;
}

void _stdcall FiberJobWrapper(void* data)
{
	Fiber* fiberData = (Fiber*)data;

	while (true)
	{
		Job* job = fiberData->currentJob;

		job->jobFunction(job->jobData);
		--job->jobCounter;
		SwitchToWorker();
	}
}
}

JobQueue::JobQueue(uint32_t size, uint32_t numFibers, uint32_t numWorkers)
	: alloc(DefAlloc())
{
	this->numWorkers = numWorkers == MATCH_NUM_LOGICAL_CORES ? GetNumLogicalCores() : numWorkers;
	queueSize = size;

	uint32_t t = max(size, numFibers);
	uintptr_t* initPtrArrayBuffer = DL_NEW_ARRAY(alloc, uintptr_t, max(size, numFibers));
	// TEST: this causes the delete to loop almost forever, why?

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

std::atomic<uint32_t>* JobQueue::Push(Job* jobs, uint32_t numJobs)
{
	Internal::AlignedCounter* jobCounter;

	counterQueue->TryPop(&jobCounter);

	for (uint32_t i = 0; i < numJobs; ++i)
		jobs[i].jobCounter = jobCounter;

	uint32_t jobsQueued = jobQueue->TryPush(jobs, numJobs);

	for (uint32_t i = jobsQueued; i < numJobs; ++i)
		jobs[i].jobCounter = nullptr;

	jobCounter->store(jobsQueued);

	return (std::atomic<uint32_t>*)jobCounter;
}

void JobQueue::WaitForCounter(std::atomic<uint32_t>* counter)
{
	// TODO: Fix this currentJob having nullptr members when it should not, probably
	PauseJob(&Internal::currentJob);

	if (Internal::isWorkerThread)
		Internal::SwitchToWorker();
	else
		WaitWithoutWork(counter);
}

void JobQueue::PauseJob(Job* job)
{
	if (numPausedJobs == queueSize)
		throw std::exception("Paused jobs queue is full");

	pausedJobs[numPausedJobs++] = *job;
}

bool JobQueue::GetReadyJob(Job* job)
{
	// Check paused jobs

	return jobQueue->TryPop(job);
}

Internal::Fiber JobQueue::CreateFiber(void* fiberData)
{
	Internal::Fiber fiber;

	fiber.currentJob = nullptr;
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

void JobQueue::WaitWithoutWork(std::atomic<uint32_t>* counter)
{
	while (*counter != 0)
		Sleep(5);
}

void JobQueue::SetupCounters(uint32_t numCounters, uintptr_t* initPtrArrayBuffer)
{
	this->numCounters = numCounters;
	counters = DL_NEW_ARRAY(DefAlloc(), Internal::AlignedCounter, numCounters);

	for (uint32_t i = 0; i < numCounters; ++i)
		initPtrArrayBuffer[i] = (uintptr_t)&counters[i];

	counterQueue = DL_NEW(DefAlloc(),
		ConcurrentQueue<Internal::AlignedCounter*>,
		numCounters,
		(Internal::AlignedCounter**)initPtrArrayBuffer,
		numCounters);
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
	numPausedJobs = 0;
	pausedJobs = DL_NEW_ARRAY(alloc, Job, size);
	readyPausedJobs = DL_NEW(alloc, ConcurrentQueue<Job>, size);
	jobQueue = DL_NEW(alloc, ConcurrentQueue<Job>, size);
}

void JobQueue::SetupWorkers(uint32_t numWorkers)
{
	workerThreads = DL_NEW_ARRAY(alloc, Thread*, numWorkers);

	workerThreadData.jobDoer = this;
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
	DL_DELETE(DefAlloc(), readyPausedJobs);
	DL_DELETE_ARRAY(DefAlloc(), pausedJobs);
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
