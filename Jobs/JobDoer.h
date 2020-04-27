#pragma once
#include <cstdint>
#include "ConcurrentQueue.h"
#include "Thread.h"

namespace DuckLib
{
namespace Internal
{
namespace Job
{
void _stdcall FiberJobWrapper(void* data);
uint32_t _stdcall WorkerThreadJob(void* data);
}
}

struct Job
{
	void (*jobFunction)(void*);
	void* jobData;
};

class JobDoer
{
public:

	JobDoer(uint32_t jobQueueSize, uint32_t numFibers);

private:

	friend uint32_t _stdcall Internal::Job::WorkerThreadJob(void* data);
	friend void _stdcall Internal::Job::FiberJobWrapper(void* data);

	static const uint8_t CACHE_LINE_SIZE = 64;

	struct Fiber
	{
		void* osFiber;
		Job* currentJob;

		// TODO: Investigate what would be more appropriate: 64000 or 65536
		static const uint32_t DEFAULT_STACK_SIZE = 65536;
	};

	struct WorkerThreadData
	{
		Thread* thread;
		JobDoer* jobDoer;
	};

	Fiber CreateFiber(void* fiberData);
	uint32_t GetNumLogicalCores() const;

	Fiber* fibers;
	uint32_t numFibers;

	uint32_t jobQueueSize;

	ConcurrentQueue<Fiber>* fiberQueue;
	ConcurrentQueue<Job>* jobQueue;

	Thread** workerThreads;
};
}