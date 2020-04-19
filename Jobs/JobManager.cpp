#include <Windows.h>
#include <atomic>
#include <cassert>
#include "JobManager.h"

namespace DuckLib
{
namespace Internal
{
namespace Job
{
std::atomic<bool> runWorkers;
thread_local void* workerFiber {};

DWORD _stdcall WorkerThreadJob(void*)
{
	workerFiber = ConvertThreadToFiber(nullptr);

	while (runWorkers)
	{ }

	// TODO: Consider resetting back to thread before quitting, or is it redundant?

	return 0;
}

void _stdcall FiberJobWrapper(void* data)
{
	JobManager::Fiber* fiberData = (JobManager::Fiber*)data;

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

JobManager::JobManager()
{
	uint32_t logicalCoreCount = GetNumLogicalCores();
	uint32_t threadId {};

	for (uint32_t i = 0; i < NUM_FIBERS; ++i)
	{
		fiberFreeList[i] = i + 1;
#ifdef _WIN32
		CreateFiber((SIZE_T)Fiber::DEFAULT_STACK_SIZE,
			(LPFIBER_START_ROUTINE)&Internal::Job::FiberJobWrapper,
			&fibers[i]);
#endif
	}

	fiberFreeList[NUM_FIBERS - 1] = INVALID_FIBER_INDEX;
	freeFiberIndex = 0;

	for (uint32_t i = 0; i < logicalCoreCount; ++i)
#ifdef _WIN32
		workerThreads[i] = CreateThread(nullptr,
			0,
			(LPTHREAD_START_ROUTINE)&Internal::Job::WorkerThreadJob,
			nullptr,
			0,
			(LPDWORD)&threadId);
#endif
}

uint32_t JobManager::GetFreeFiberIndex()
{
	uint8_t fiberIndex;
	uint8_t newNextFiberIndex;

	do
	{
		fiberIndex = freeFiberIndex.load();
		newNextFiberIndex = fiberFreeList[fiberIndex];

		assert(newNextFiberIndex != INVALID_FIBER_INDEX);
	}
	while (!freeFiberIndex.compare_exchange_strong(fiberIndex, newNextFiberIndex));

	return fiberIndex;
}

void JobManager::ReturnFiber(uint8_t fiberIndex)
{
	uint8_t currentFreeFiberIndex;

	do
	{
		currentFreeFiberIndex = freeFiberIndex.load();
		fiberFreeList[fiberIndex] = currentFreeFiberIndex;
	}
	while (!freeFiberIndex.compare_exchange_strong(currentFreeFiberIndex, fiberIndex));
}

uint32_t JobManager::GetNumLogicalCores() const
{
#ifdef _WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
#endif
}
}