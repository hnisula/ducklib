#include <iostream>
#include <conio.h>
#include "../../Threading/JobQueue.h"

using namespace DuckLib;

std::atomic<uint32_t> numJobsCompleted {0};
uint32_t* jobItems;
const uint32_t NUM_CHILD_JOBS = 8;
const uint32_t NUM_PAUSE_JOBS = 1024;
const uint32_t PAUSE_QUEUE_SIZE = 512;
const uint32_t NUM_PAUSE_FIBERS = 256;

JobQueue jobQueue(PAUSE_QUEUE_SIZE, NUM_PAUSE_FIBERS);

struct ChildJobData
{
	uint32_t result;
	uint32_t index;
};

struct PauseJobData
{	
	ChildJobData childJobData[NUM_CHILD_JOBS];
};

void JobFunc(void* data)
{
	uint32_t* jobItem = (uint32_t*)data;

	*jobItem = 0;
	
	++numJobsCompleted;
}

void ChildPauseJobFunc(void* data)
{
	ChildJobData* jobData = (ChildJobData*)data;

	jobData->result = jobData->index * 10 + 2;
}

void PauseJobFunc(void* data)
{
	PauseJobData* jobData = (PauseJobData*)data;
	Job* jobs = DL_NEW_ARRAY(DefAlloc(), Job, NUM_CHILD_JOBS);

	for (uint32_t i = 0; i < NUM_CHILD_JOBS; ++i)
	{
		jobData->childJobData[i].result = 0;
		jobData->childJobData[i].index = i + 1;

		jobs[i] = {&ChildPauseJobFunc, &jobData->childJobData[i]};
	}

	std::atomic<uint32_t>* counter = jobQueue.Push(jobs, NUM_CHILD_JOBS);

	jobQueue.WaitForCounter(counter);
}

void InitJobData(uint32_t numJobs)
{
	jobItems = DL_NEW_ARRAY(DefAlloc(), uint32_t, numJobs);
	
	for (uint32_t i = 0; i < numJobs; ++i)
		jobItems[i] = i + 1;
}

Job* GenerateJobs(uint32_t numJobs)
{
	Job* jobs = DL_NEW_ARRAY(DefAlloc(), Job, numJobs);

	for (uint32_t i = 0; i < numJobs; ++i)
		jobs[i] = {&JobFunc, &jobItems[i]};
	
	return jobs;
}

bool CheckJobResult(uint32_t numJobs)
{
	for (uint32_t i = 0; i < numJobs; ++i)
		if (jobItems[i] != 0)
			return false;
	return true;
}

void CleanupJobData(uint32_t numJobs)
{
	DL_DELETE_ARRAY(DefAlloc(), jobItems);
}

void NoPauseTest()
{
	const uint32_t jobQueueSize = 512;
	const uint32_t numJobs = 512;

	static_assert(numJobs <= jobQueueSize,
		"numJobs must be less than queue size as there is just one pass for submitting jobs");

	InitJobData(numJobs);

	Job* jobs = GenerateJobs(numJobs);

	for (uint32_t i = 0; i < numJobs; ++i)
		jobQueue.Push(&jobs[i], 1);

	while (numJobsCompleted.load() < numJobs)
		YieldThread(100);

	if (!CheckJobResult(numJobs))
		std::cout << "ERROR: Job result is incorrect" << std::endl;
	else
		std::cout << "Jobs executed successfully" << std::endl;

	CleanupJobData(numJobs);
}

void PauseTest()
{
	PauseJobData* pauseJobData = DL_NEW_ARRAY(DefAlloc(), PauseJobData, NUM_PAUSE_JOBS);
	Job* pauseJobs = DL_NEW_ARRAY(DefAlloc(), Job, NUM_PAUSE_JOBS);
	
	for (uint32_t i = 0; i < NUM_PAUSE_JOBS; ++i)
		pauseJobs[i] = {&PauseJobFunc, &pauseJobData[i]};
	
	std::atomic<uint32_t>* counter = jobQueue.Push(pauseJobs, NUM_PAUSE_JOBS);
	
	jobQueue.WaitForCounter(counter);
}

int main()
{
	// NoPauseTest();
	PauseTest();

	_getch();

	return 0;
}