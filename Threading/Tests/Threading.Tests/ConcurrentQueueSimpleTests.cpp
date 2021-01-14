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