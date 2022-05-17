#pragma once
#include <cstdint>
#include "ConcurrentQueue.h"
#include "Thread.h"

namespace DuckLib
{
struct JobCounter;
class JobQueue;

namespace Internal
{
void SwitchToWorker();
void InitWorkerThread();
uint32_t __stdcall WorkerThreadJob(void* data);
void __stdcall FiberJobWrapper(void* data);
}

struct Job
{
	Job();
	Job(void (*jobFunction)(void*), void* jobData);

private:

	friend void __stdcall Internal::FiberJobWrapper(void*);
	friend uint32_t __stdcall Internal::WorkerThreadJob(void*);
	friend class JobQueue;

	JobCounter* jobCounter;
	void (*jobFunction)(void*);
	void* jobData;
};

namespace Internal
{
void _stdcall FiberJobWrapper(void*);
	
struct alignas(CACHE_LINE_SIZE) Fiber
{
	Job currentJob;
	void* osFiber;

	static const uint32_t DEFAULT_STACK_SIZE = 65536;
};

void SwitchFiber(const Fiber* fiber);
}

struct alignas(CACHE_LINE_SIZE) JobCounter
{
	friend void _stdcall Internal::FiberJobWrapper(void*);
	friend class JobQueue;

protected:

	void Reset();
	void Decrement();
	
	JobQueue* jobQueue;
	std::atomic<uint32_t> counter;
	Internal::Fiber* waitingJobFiber;
};

class JobQueue
{
public:

	static constexpr uint32_t MATCH_NUM_LOGICAL_CORES = 0;
	
	JobQueue(uint32_t size, uint32_t numFibers, uint32_t numWorkers = MATCH_NUM_LOGICAL_CORES);
	~JobQueue();

	JobCounter* Push(Job* jobs, uint32_t numJobs);

	void WaitForCounter(JobCounter* counter);

private:

	friend struct Internal::Fiber;
	friend struct JobCounter;
	friend uint32_t __stdcall Internal::WorkerThreadJob(void* data);
	friend void __stdcall Internal::FiberJobWrapper(void* data);

	struct WorkerThreadData
	{
		JobQueue* jobQueue;
		std::atomic<bool> runFlag;
		std::atomic<bool> startFlag;
	};

	Internal::Fiber* GetReadyJobAndFiber();
	void ReturnFiberIfJobCompleted(Internal::Fiber* completedFiber);

	void FinalizeCompletedJobCounter(JobCounter* counter);

	Internal::Fiber CreateFiber(void* fiberData);
	void DeleteFiber(Internal::Fiber* fiber);
	uint32_t GetNumLogicalCores() const;

	void WaitIdle(const JobCounter* counter);

	void SetupCounters(uint32_t numCounters, uintptr_t* initPtrArrayBuffer);
	void SetupFibers(uint32_t numFibers, uintptr_t* initPtrArrayBuffer);
	void SetupJobStorage(uint32_t size);
	void SetupWorkers(uint32_t numWorkers);

	void TearDownWorkers();
	void TearDownJobStorage();
	void TearDownFibers();
	void TearDownCounters();

	IAlloc* alloc;

	uint32_t numCounters;
	JobCounter* counters;
	ConcurrentQueue<JobCounter*>* counterQueue;
	
	uint32_t numFibers;
	Internal::Fiber* fibers;
	ConcurrentQueue<Internal::Fiber*>* fiberQueue;

	uint32_t queueSize;
	ConcurrentQueue<Internal::Fiber*>* readyPausedJobFiberQueue;
	ConcurrentQueue<Job>* jobQueue;

	uint32_t numWorkers;
	Thread** workerThreads;
	WorkerThreadData workerThreadData;

	// TODO: Implement
#ifdef _DEBUG
	uint32_t maxNumFibers;
	uint32_t maxNumJobs;
	uint32_t maxNumPausedJobs;
	uint32_t maxNumJobCounters;
#endif
};
}