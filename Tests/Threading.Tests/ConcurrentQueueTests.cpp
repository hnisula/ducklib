#include <gtest/gtest.h>
#include "ConcurrentQueue.h"
#include "Thread.h"

using namespace DuckLib;

const uint32_t pushValue = 420;

uint32_t PushThread(void* data)
{
	ConcurrentQueue<uint32_t>* queue = (ConcurrentQueue<uint32_t>*)data;

	queue->TryPush(pushValue);

	return 0;
}

TEST(ConcurrentQueueTest, TryPushSingle)
{
	ConcurrentQueue<uint32_t> queue(128);
	uint32_t result;

	queue.TryPush(200);
	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_TRUE(result == 200);
}

TEST(ConcurrentQueueTest, TryPushMultiple)
{
	ConcurrentQueue<uint32_t> queue(128);
	uint32_t result;

	queue.TryPush(200);
	queue.TryPush(100);
	queue.TryPush(69);
	queue.TryPush(420);
	queue.TryPush(1337);

	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_TRUE(result == 200);
	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_TRUE(result == 100);
	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_TRUE(result == 69);
	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_TRUE(result == 420);
	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_TRUE(result == 1337);
}

TEST(ConcurrentQueueTest, TryPopEmpty)
{
	ConcurrentQueue<uint32_t> queue(128);
	uint32_t result;

	EXPECT_FALSE(queue.TryPop(&result));
}

TEST(ConcurrentQueueTest, TryPushFull)
{
	ConcurrentQueue<uint32_t> queue(2);

	EXPECT_TRUE(queue.TryPush(1));
	EXPECT_TRUE(queue.TryPush(2));
	EXPECT_FALSE(queue.TryPush(3));
}

TEST(ConcurrentQueueTest, TryPopAfterPushFull)
{
	ConcurrentQueue<uint32_t> queue(2);
	uint32_t result {};

	EXPECT_TRUE(queue.TryPush(1));
	EXPECT_TRUE(queue.TryPush(2));
	EXPECT_FALSE(queue.TryPush(3));

	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_EQ(1, result);
	EXPECT_TRUE(queue.TryPop(&result));
	EXPECT_EQ(2, result);
	EXPECT_FALSE(queue.TryPop(&result));
}

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
	uint32_t* poppedItems = DL_NEW_ARRAY(DefAlloc(), uint32_t, config->numItems);
	uint32_t numPopped = 0;
	uint64_t counter = config->pushCounter->load();
	uint64_t pushReservation = config->push ? (*config->pushReservation)++ : (uint64_t)-1;

	while (counter < config->numItems || pushReservation < config->numItems || !queueIsEmpty)
	{
		Sleep(10);
		
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

	DL_DELETE_ARRAY(DefAlloc(), poppedItems);
	
	return 0;
}

TEST(ConcurrentQueueTest, ConcurrentTryPush)
{
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

	for (uint32_t i = 0; i < numPushers; ++i)
		workers[numWorkers + i] = DL_NEW(DefAlloc(), Thread, &QueueWorker,
			&WorkerConfig { &queue, &pushCounter, &pushReservation, items, allPoppedItems, numItems,
				&numAllPoppedItems, true, false });

	numWorkers += numPushers;

	for (uint32_t i = 0; i < numPoppers; ++i)
		workers[numWorkers + i] = DL_NEW(DefAlloc(), Thread, &QueueWorker,
			&WorkerConfig {&queue, &pushCounter, &pushReservation, items, allPoppedItems, numItems,
			&numAllPoppedItems, false, true});

	numWorkers += numPoppers;

	for (uint32_t i = 0; i < numHybrids; ++i)
		workers[numWorkers + i] = DL_NEW(DefAlloc(), Thread, &QueueWorker,
			&WorkerConfig {&queue, &pushCounter, &pushReservation, items, allPoppedItems, numItems,
			&numAllPoppedItems, true, true});

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

		EXPECT_NE((uint32_t)-1, matchedIndex);
	}
}