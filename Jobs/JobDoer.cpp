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
thread_local void* workerFiber {};

uint32_t _stdcall WorkerThreadJob(void* data)
{
	JobDoer::WorkerThreadData* workerThreadData = (JobDoer::WorkerThreadData*)data;
	JobDoer* jobDoer = workerThreadData->jobDoer;
	Thread* thread = workerThreadData->thread;
	workerFiber = ConvertThreadToFiber(nullptr);

	while (runWorkers)
	{
		JobDoer::Fiber fiber;
		DuckLib::Job job;

		if (!jobDoer->jobQueue->TryPop(&job))
		{
			thread->Sleep(10);
			continue;
		}
		
		// TODO: Decide what to do if popping one fails
		jobDoer->fiberQueue->TryPop(&fiber);
		fiber.currentJob = &job;
		SwitchToFiber(fiber.osFiber);
		// TODO: Decide what to do if pushing one fails
		jobDoer->fiberQueue->TryPush(fiber);
	}

	// TODO: Consider resetting back to thread before quitting, or is it redundant?

	return 0;
}

void _stdcall FiberJobWrapper(void* data)
{
	JobDoer::Fiber* fiberData = (JobDoer::Fiber*)data;

	while (true)
	{
		DuckLib::Job* job = fiberData->currentJob;

		job->jobFunction(job->jobData);
#ifdef _WIN32
		SwitchToFiber(workerFiber);
#endif
	}
}
}
}

JobDoer::JobDoer(uint32_t jobQueueSize, uint32_t numFibers)
	: numFibers(numFibers)
, jobQueueSize(jobQueueSize)
{
	uint32_t logicalCoreCount = GetNumLogicalCores();
	fibers = DL_NEW_ARRAY(DefAlloc(), Fiber, numFibers);

	for (uint32_t i = 0; i < numFibers; ++i)
	{
		fibers[i] = CreateFiber(&fibers[i]);
	}

	fiberQueue = DL_NEW(DefAlloc(), ConcurrentQueue<Fiber>, numFibers, fibers, numFibers);
	workerThreads = DL_NEW_ARRAY(DefAlloc(), Thread*, logicalCoreCount);

	for (uint32_t i = 0; i < logicalCoreCount; ++i)
		workerThreads[i] = DL_NEW(DefAlloc(), Thread, Internal::Job::WorkerThreadJob, nullptr);

	jobQueue = DL_NEW(DefAlloc(), ConcurrentQueue<Job>, jobQueueSize);
}

JobDoer::Fiber JobDoer::CreateFiber(void* fiberData)
{
	Fiber fiber;

	fiber.currentJob = nullptr;
#ifdef _WIN32
	fiber.osFiber = ::CreateFiber((SIZE_T)Fiber::DEFAULT_STACK_SIZE,
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