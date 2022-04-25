#include <iostream>
#include <conio.h>

#include "../../Threading/JobQueue.h"

using namespace DuckLib;

std::atomic<uint32_t> numJobsCompleted {0};
uint32_t* jobItems;
const uint32_t NUM_JOBS_NOPAUSE = 512;
const uint32_t NUM_CHILD_JOBS = 16;
const uint32_t NUM_PAUSE_JOBS = 16;
const uint32_t QUEUE_SIZE = 1024;
const uint32_t NUM_FIBERS = 512;
uint32_t pausePushCounter = 0;

JobQueue jobQueue(QUEUE_SIZE, NUM_FIBERS);

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
	Job* jobs = DefAlloc()->Allocate<Job>(NUM_CHILD_JOBS);

	for (uint32_t i = 0; i < NUM_CHILD_JOBS; ++i)
	{
		jobData->childJobData[i].result = 0;
		jobData->childJobData[i].index = i + 1;

		jobs[i] = {&ChildPauseJobFunc, &jobData->childJobData[i]};
	}

	pausePushCounter++;
	JobCounter* counter = jobQueue.Push(jobs, NUM_CHILD_JOBS);

	jobQueue.WaitForCounter(counter);
}

void InitJobData(uint32_t numJobs)
{
	jobItems = DefAlloc()->Allocate<uint32_t>(numJobs);
	
	for (uint32_t i = 0; i < numJobs; ++i)
		jobItems[i] = i + 1;
}

Job* GenerateJobs(uint32_t numJobs)
{
	Job* jobs = DefAlloc()->Allocate<Job>(numJobs);

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

bool CheckPauseJobResults(PauseJobData* jobData)
{
	for (uint32_t i = 0; i < NUM_PAUSE_JOBS; ++i)
		for (uint32_t u = 0; u < NUM_CHILD_JOBS; ++u)
			if (jobData[i].childJobData[u].result != jobData[i].childJobData[u].index * 10 + 2)
				return false;

	return true;
}

void CleanupJobData(uint32_t numJobs)
{
	DefAlloc()->Free(jobItems);
}

void NoPauseTest()
{
	InitJobData(NUM_JOBS_NOPAUSE);

	Job* jobs = GenerateJobs(NUM_JOBS_NOPAUSE);

	for (uint32_t i = 0; i < NUM_JOBS_NOPAUSE; ++i)
		jobQueue.Push(&jobs[i], 1);

	while (numJobsCompleted.load() < NUM_JOBS_NOPAUSE)
		YieldThread(100);

	if (!CheckJobResult(NUM_JOBS_NOPAUSE))
		std::cout << "ERROR: Job result is incorrect" << std::endl;
	else
		std::cout << "Jobs executed successfully" << std::endl;

	CleanupJobData(NUM_JOBS_NOPAUSE);
}

void PauseTest()
{
	PauseJobData* pauseJobData = DefAlloc()->Allocate<PauseJobData>(NUM_PAUSE_JOBS);
	Job* pauseJobs = DefAlloc()->Allocate<Job>(NUM_PAUSE_JOBS);
	
	for (uint32_t i = 0; i < NUM_PAUSE_JOBS; ++i)
		pauseJobs[i] = {&PauseJobFunc, &pauseJobData[i]};
	
	JobCounter* counter = jobQueue.Push(pauseJobs, NUM_PAUSE_JOBS); // check if counter ever changes. Also check workers if they actually do any job
	
	jobQueue.WaitForCounter(counter);

	if (!CheckPauseJobResults(pauseJobData))
		std::cout << "Data check failed!" << std::endl;
	else
		std::cout << "Data check passed!" << std::endl;
}

int main()
{
	// NoPauseTest();
	PauseTest();

	_getch();

	return 0;
}