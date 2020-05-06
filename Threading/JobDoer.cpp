#include <Windows.h>
#include <atomic>
#include <cassert>
#include "JobDoer.h"

namespace DuckLib
{
namespace Internal
{
namespace Job
{
std::atomic<bool> runWorkers;
thread_local Fiber workerThreadFiber {};

void SwitchFiber(Fiber* fiber)
{
#ifdef _WIN32
	SwitchToFiber(fiber->osFiber);
#endif
}

void InitWorkerThread()
{
#ifdef _WIN32
	workerThreadFiber.osFiber = ConvertThreadToFiber(&workerThreadFiber);
#endif
}

uint32_t _stdcall WorkerThreadJob(void* data)
{
	JobDoer::WorkerThreadData* workerThreadData = (JobDoer::WorkerThreadData*)data;
	std::atomic<bool>& runFlag = workerThreadData->runFlag;
	std::atomic<bool>& startFlag = workerThreadData->startFlag;
	JobDoer* jobDoer = workerThreadData->jobDoer;

	InitWorkerThread();

	while (!startFlag.load())
		YieldThread(10);

	while (runFlag.load())
	{
		Fiber* jobFiber;
		DuckLib::Job job;

		if (!jobDoer->jobQueue->TryPop(&job))
		{
			YieldThread(10);
			continue;
		}
		
		// TODO: Decide what to do if popping or pushing fails
		jobDoer->fiberQueue->TryPop(&jobFiber);
		jobFiber->currentJob = &job;
		SwitchFiber(jobFiber);
		jobDoer->fiberQueue->TryPush(jobFiber);
	}

	return 0;
}

void _stdcall FiberJobWrapper(void* data)
{
	Fiber* fiberData = (Fiber*)data;

	while (true)
	{
		DuckLib::Job* job = fiberData->currentJob;

		job->jobFunction(job->jobData);
		SwitchFiber(&workerThreadFiber);
	}
}
}
}

JobDoer::JobDoer(uint32_t jobQueueSize, uint32_t numFibers)
	: alloc(DefAlloc())
{
	numWorkers = GetNumLogicalCores();
	fibers = DL_NEW_ARRAY(alloc, Internal::Job::Fiber, numFibers);
	Internal::Job::Fiber** fiberPtrs = DL_NEW_ARRAY(alloc, Internal::Job::Fiber*, numFibers);

	for (uint32_t i = 0; i < numFibers; ++i)
	{
		fibers[i] = CreateFiber(&fibers[i]);
		fiberPtrs[i] = &fibers[i];
	}

	fiberQueue = DL_NEW(alloc,
		ConcurrentQueue<Internal::Job::Fiber*>,
		numFibers,
		fiberPtrs,
		numFibers);

	DL_DELETE_ARRAY(alloc, fiberPtrs);
	
	workerThreads = DL_NEW_ARRAY(alloc, Thread*, numWorkers);

	workerThreadData.jobDoer = this;
	workerThreadData.runFlag.store(true);
	workerThreadData.startFlag.store(false);

	for (uint32_t i = 0; i < numWorkers; ++i)
		workerThreads[i] = DL_NEW(alloc,
			Thread,
			Internal::Job::WorkerThreadJob,
			&workerThreadData);

	jobQueue = DL_NEW(alloc, ConcurrentQueue<Job>, jobQueueSize);

	workerThreadData.startFlag.store(true);
}

JobDoer::~JobDoer()
{
	workerThreadData.runFlag.store(false);

	for (uint32_t i = 0; i < numWorkers; ++i)
	{
		workerThreads[i]->Join();
		DL_DELETE(alloc, workerThreads[i]);
	}

	DL_DELETE_ARRAY(alloc, workerThreads);
	
	assert(fibers);
	assert(fiberQueue);
	assert(jobQueue);
	
	DL_DELETE_ARRAY(alloc, fibers);
	DL_DELETE(alloc, fiberQueue);
	DL_DELETE(alloc, jobQueue);
}

bool JobDoer::Queue(Job* job)
{
	return jobQueue->TryPush(*job);
}

Internal::Job::Fiber JobDoer::CreateFiber(void* fiberData)
{
	Internal::Job::Fiber fiber;

	fiber.currentJob = nullptr;
#ifdef _WIN32
	fiber.osFiber = ::CreateFiber((SIZE_T)Internal::Job::Fiber::DEFAULT_STACK_SIZE,
		(LPFIBER_START_ROUTINE)&Internal::Job::FiberJobWrapper,
		fiberData);
#endif

	return fiber;
}

uint32_t JobDoer::GetNumLogicalCores() const
{
#ifdef _WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
#endif
}
}
