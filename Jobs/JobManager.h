#pragma once
#include <cstdint>
#include "ConcurrentQueue.h"

namespace DuckLib
{
namespace Internal
{
namespace Job
{
void _stdcall FiberJobWrapper(void* data);
DWORD _stdcall WorkerThreadJob(void* dummy);
}
}

struct Job
{
	void (*jobFunction)(void*);
	void* jobData;
};

class JobManager
{
public:

	JobManager();

private:

	friend DWORD _stdcall Internal::Job::WorkerThreadJob(void* dummy);
	friend void _stdcall Internal::Job::FiberJobWrapper(void* data);

	static const uint32_t NUM_FIBERS = 128;
	static const uint32_t MAX_NUM_WORKER_THREADS = 64;
	static const uint8_t INVALID_FIBER_INDEX = (uint8_t)-1;
	static const uint8_t CACHE_LINE_SIZE = 64;

	struct Fiber
	{
		void* osFiber;
		Job* currentJob;

		// TODO: Investigate what would be more appropriate: 64000 or 65536
		static const uint32_t DEFAULT_STACK_SIZE = 65536;
	};

	uint32_t GetFreeFiberIndex();
	void ReturnFiber(uint8_t fiberIndex);
	uint32_t GetNumLogicalCores() const;

	Fiber fibers[NUM_FIBERS];
	uint8_t fiberFreeList[NUM_FIBERS];
	std::atomic<uint8_t> freeFiberIndex;

	HANDLE workerThreads[MAX_NUM_WORKER_THREADS];
};
}