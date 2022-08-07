#include <gtest/gtest.h>
#include "Core/Memory/Containers/Iterators.h"

using namespace DuckLib;

class Foo
{
public:

	uint32 v1;
	uint32 v2;
	uint32* p;
};

TEST(IteratorsTest, TArrayRangedLoop)
{
	// TODO: Support for braced initialization list would be handy here
	TArray<uint32> a;

	for (uint32 i = 0; i < 10; ++i)
		a.Append(i);

	uint32 n = 0;

	for (uint32 i : a)
		EXPECT_EQ(n++, i);
}

TEST(IteratorsTest, TArrayRangedLoopEmpty)
{
	TArray<uint32> a;

	for (uint32 i : a)
		EXPECT_TRUE(false);
}
