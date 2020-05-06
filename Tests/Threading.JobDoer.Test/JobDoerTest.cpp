#include <iostream>
#include <conio.h>
#include "../../Threading/JobDoer.h"

using namespace DuckLib;

std::atomic<uint32_t> numJobsCompleted {0};
uint32_t* jobItems;

void JobFunc(void* data)
{
	uint32_t* jobItem = (uint32_t*)data;

	*jobItem = 0;
	
	++numJobsCompleted;
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
	{
		jobs[i].jobFunction = &JobFunc;
		jobs[i].jobData = &jobItems[i];
	}
	
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

int main()
{
	const uint32_t jobQueueSize = 512;
	const uint32_t numFibers = 128;
	const uint32_t numJobs = 512;

	static_assert(numJobs <= jobQueueSize,
		"numJobs must be less than queue size as there is just one pass for submitting jobs");

	InitJobData(numJobs);
	
	JobDoer jobDoer(jobQueueSize, numFibers);

	Job* jobs = GenerateJobs(numJobs);

	for (uint32_t i = 0; i < numJobs; ++i)
		jobDoer.Queue(&jobs[i]);

	while (numJobsCompleted.load() < numJobs)
		YieldThread(100);

	if (!CheckJobResult(numJobs))
		std::cout << "ERROR: Job result is incorrect" << std::endl;
	else
		std::cout << "Jobs executed successfully" << std::endl;

	CleanupJobData(numJobs);

	_getch();

	return 0;
}