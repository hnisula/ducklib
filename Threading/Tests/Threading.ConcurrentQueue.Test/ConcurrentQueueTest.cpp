#include <conio.h>
#include <exception>
#include <iostream>
#include "../../ConcurrentQueue.h"
#include "../../Thread.h"

using namespace DuckLib;

struct WorkerConfig
{
	ConcurrentQueue<uint32_t>* queue;
	std::atomic<uint32_t>* pushCounter;
	std::atomic<uint32_t>* pushReservation;
	uint32_t* items;
	uint32_t* allPoppedItems;
	uint32_t numItems;
	std::atomic<uint32_t>* numAllPoppedItems;
	bool push;
	bool pop;
};

uint32_t QueueWorker(void* configData)
{
	WorkerConfig* config = (WorkerConfig*)configData;
	bool queueIsEmpty = !config->pop;
	uint32_t value {};
	uint32_t* poppedItems = DefAlloc()->Allocate<uint32_t>(config->numItems);
	uint32_t numPopped = 0;
	uint64_t counter = config->pushCounter->load();
	uint64_t pushReservation = config->push ? (*config->pushReservation)++ : (uint64_t)-1;

	while (counter < config->numItems || pushReservation < config->numItems || !queueIsEmpty)
	{
		YieldThread(10);

		if (config->push && pushReservation < config->numItems)
			if (config->queue->TryPush(config->items[pushReservation]))
			{
				pushReservation = (*config->pushReservation)++;
				++(*config->pushCounter);
			}

		if (config->pop)
		{
			queueIsEmpty = !config->queue->TryPop(&value);

			if (!queueIsEmpty)
				poppedItems[numPopped++] = value;
		}

		counter = config->pushCounter->load();
	}

	uint32_t startTotalPoppedIndex = config->numAllPoppedItems->fetch_add(numPopped);

	for (uint32_t i = 0; i < numPopped; ++i)
		config->allPoppedItems[startTotalPoppedIndex + i] = poppedItems[i];

	DefAlloc()->Free(poppedItems);

	return 0;
}

int main()
{
	// TODO: Get these from console input or at least print them when running
	const uint32_t numPushers = 8;
	const uint32_t numPoppers = 8;
	const uint32_t numHybrids = 4;
	const uint32_t queueSize = 2;
	const uint32_t numItems = 256;

	static_assert(numItems < (uint32_t)-1, "numItems must be at least 1 less");

	ConcurrentQueue<uint32_t> queue(queueSize);
	std::atomic<uint32_t> pushCounter {0};
	std::atomic<uint32_t> pushReservation {0};
	uint32_t items[numItems];
	uint32_t allPoppedItems[numItems];
	std::atomic<uint32_t> numAllPoppedItems {0};
	Thread* workers[numPushers + numPoppers + numHybrids];
	uint32_t numWorkers = 0;

	for (uint32_t i = 0; i < numItems; ++i)
		items[i] = numItems - i;

	// TODO: Fix temp object ptr being passed as arg
	static WorkerConfig pusherConfig =
	{
		&queue,
		&pushCounter,
		&pushReservation,
		items,
		allPoppedItems,
		numItems,
		&numAllPoppedItems,
		true,
		false
	};
	static WorkerConfig popperConfig =
	{
		&queue,
		&pushCounter,
		&pushReservation,
		items,
		allPoppedItems,
		numItems,
		&numAllPoppedItems,
		false,
		true
	};
	static WorkerConfig hybridConfig =
	{
		&queue,
		&pushCounter,
		&pushReservation,
		items,
		allPoppedItems,
		numItems,
		&numAllPoppedItems,
		true,
		true
	};
	
	for (uint32_t i = 0; i < numPushers; ++i)
		workers[numWorkers + i] = DefAlloc()->New<Thread>(&QueueWorker, &pusherConfig);

	numWorkers += numPushers;

	for (uint32_t i = 0; i < numPoppers; ++i)
		workers[numWorkers + i] = DefAlloc()->New<Thread>(&QueueWorker, &popperConfig);

	numWorkers += numPoppers;

	for (uint32_t i = 0; i < numHybrids; ++i)
		workers[numWorkers + i] = DefAlloc()->New<Thread>(&QueueWorker, &hybridConfig);

	numWorkers += numHybrids;

	for (uint32_t i = 0; i < numWorkers; ++i)
		workers[i]->Join();

	bool itemIsUsed[numItems];

	for (uint32_t i = 0; i < numItems; ++i)
		itemIsUsed[i] = false;

	for (uint32_t i = 0; i < numItems; ++i)
	{
		uint32_t matchedIndex = (uint32_t)-1;

		for (uint32_t u = 0; u < numItems; ++u)
			if (!itemIsUsed[u] && items[i] == allPoppedItems[u])
			{
				matchedIndex = u;
				itemIsUsed[u] = true;
				break;
			}

		if (matchedIndex == (uint32_t)-1)
			throw std::exception("ERROR: Item missing!");
	}

	std::cout << "Test completed successfully" << std::endl;

	_getch();

	return 0;
}