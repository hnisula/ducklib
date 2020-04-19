#include <gtest/gtest.h>
#include "ConcurrentQueue.h"

using namespace DuckLib;

TEST(ConcurrentQueueTest, TryPushSingle)
{
	ConcurrentQueue<uint32_t> queue(128);
	uint32_t result;

	queue.TryPush(200);
	EXPECT_TRUE(queue.TryPop(result));
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

	EXPECT_TRUE(queue.TryPop(result));
	EXPECT_TRUE(result == 200);
	EXPECT_TRUE(queue.TryPop(result));
	EXPECT_TRUE(result == 100);
	EXPECT_TRUE(queue.TryPop(result));
	EXPECT_TRUE(result == 69);
	EXPECT_TRUE(queue.TryPop(result));
	EXPECT_TRUE(result == 420);
	EXPECT_TRUE(queue.TryPop(result));
	EXPECT_TRUE(result == 1337);
}