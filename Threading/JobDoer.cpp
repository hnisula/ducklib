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
thread_local Fiber* workerThreadFiber {};

void SwitchFiber(Fiber* fiber)
{
#ifdef _WIN32
	SwitchToFiber(fiber->osFiber);
#endif
}

void InitWorkerThread()
{
#ifdef _WIN32
	workerThreadFiber->osFiber = ConvertThreadToFiber(workerThreadFiber);
#endif
}

uint32_t _stdcall WorkerThreadJob(void* data)
{
	JobDoer::WorkerThreadData* workerThreadData = (JobDoer::WorkerThreadData*)data;
	JobDoer* jobDoer = workerThreadData->jobDoer;
	Thread* thread = workerThreadData->thread;

	InitWorkerThread();

	while (runWorkers)
	{
		Fiber jobFiber;
		DuckLib::Job job;

		if (!jobDoer->jobQueue->TryPop(&job))
		{
			thread->Sleep(10);
			continue;
		}

		// TODO: Decide what to do if popping or pushing fails
		jobDoer->fiberQueue->TryPop(&jobFiber);
		jobFiber.currentJob = &job;
		SwitchFiber(&jobFiber);
		jobDoer->fiberQueue->TryPush(jobFiber);
	}

	// TODO: Consider resetting back to thread before quitting, or is it redundant?

	return 0;
}

void _stdcall FiberJobWrapper(void* data)
{
	Fiber* fiberData = (Fiber*)data;

	while (true)
	{
		DuckLib::Job* job = fiberData->currentJob;

		job->jobFunction(job->jobData);
		SwitchFiber(workerThreadFiber);
	}
}
}
}

JobDoer::JobDoer(uint32_t jobQueueSize, uint32_t numFibers)
	: numFibers(numFibers)
	, jobQueueSize(jobQueueSize)
{
	uint32_t logicalCoreCount = GetNumLogicalCores();
	fibers = DL_NEW_ARRAY(DefAlloc(), Internal::Job::Fiber, numFibers);

	for (uint32_t i = 0; i < numFibers; ++i)
	{
		fibers[i] = CreateFiber(&fibers[i]);
	}

	fiberQueue = DL_NEW(DefAlloc(),
		ConcurrentQueue<Internal::Job::Fiber>,
		numFibers,
		fibers,
		numFibers);
	workerThreads = DL_NEW_ARRAY(DefAlloc(), Thread*, logicalCoreCount);

	for (uint32_t i = 0; i < logicalCoreCount; ++i)
		workerThreads[i] = DL_NEW(DefAlloc(), Thread, Internal::Job::WorkerThreadJob, nullptr);

	jobQueue = DL_NEW(DefAlloc(), ConcurrentQueue<Job>, jobQueueSize);
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
