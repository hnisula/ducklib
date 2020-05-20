#pragma once
#include <cstdint>
#include "ConcurrentQueue.h"
#include "Thread.h"

namespace DuckLib
{
class JobQueue;
struct Job;

namespace Internal
{
class alignas(CACHE_LINE_SIZE) AlignedCounter
{
	std::atomic<uint32_t> counter;
	Job* waitingJob;
};

struct alignas(CACHE_LINE_SIZE) Fiber
{
	void* osFiber;
	Job* currentJob;

	static const uint32_t DEFAULT_STACK_SIZE = 65536;
};

void SwitchFiber(Fiber* fiber);
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
	friend class JobQueue;

	void (*jobFunction)(void*);
	void* jobData;
	std::atomic<uint32_t>* jobCounter;
};

class JobQueue
{
public:

	static const uint32_t MATCH_NUM_LOGICAL_CORES = 0;
	
	JobQueue(uint32_t size, uint32_t numFibers, uint32_t numWorkers = MATCH_NUM_LOGICAL_CORES);
	~JobQueue();

	std::atomic<uint32_t>* Push(Job* jobs, uint32_t numJobs);

	void WaitForCounter(std::atomic<uint32_t>* counter);

private:

	friend struct Internal::Fiber;
	friend uint32_t __stdcall Internal::WorkerThreadJob(void* data);
	friend void __stdcall Internal::FiberJobWrapper(void* data);

	struct WorkerThreadData
	{
		JobQueue* jobDoer;
		std::atomic<bool> runFlag;
		std::atomic<bool> startFlag;
	};

	void PauseJob(Job* job);
	bool GetReadyJob(Job* job);

	Internal::Fiber CreateFiber(void* fiberData);
	void DeleteFiber(Internal::Fiber* fiber);
	uint32_t GetNumLogicalCores() const;

	void WaitWithoutWork(std::atomic<uint32_t>* counter);

	void SetupCounters(uint32_t numCounters, uintptr_t* initPtrArrayBuffer);
	void SetupFibers(uint32_t numFibers, uintptr_t* initPtrArrayBuffer);
	void SetupJobStorage(uint32_t size);
	void SetupWorkers(uint32_t numWorkers);

	void TearDownWorkers();
	void TearDownJobStorage();
	void TearDownFibers();
	void TearDownCounters();

	IAlloc& alloc;

	uint32_t numCounters;
	Internal::AlignedCounter* counters;
	ConcurrentQueue<Internal::AlignedCounter*>* counterQueue;
	
	uint32_t numFibers;
	Internal::Fiber* fibers;
	ConcurrentQueue<Internal::Fiber*>* fiberQueue;

	uint32_t numPausedJobs;
	uint32_t queueSize;
	Job* pausedJobs;
	ConcurrentQueue<Job>* readyPausedJobs;
	ConcurrentQueue<Job>* jobQueue;

	uint32_t numWorkers;
	Thread** workerThreads;
	WorkerThreadData workerThreadData;
};
}