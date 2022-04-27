#include <gtest/gtest.h>
#include <Core/Memory/Containers/TArray.h>

using namespace DuckLib;

TEST(TArrayTests, Simple)
{
	TArray<uint32_t> a;

	a.Append(1);

	EXPECT_EQ(1, a.Size());
	EXPECT_EQ(1, a[0]);
	EXPECT_LE(1, a.Capacity());
}

TEST(TArrayTests, MultipleSizeIncreases)
{
	TArray<uint32_t> a;
	constexpr uint32_t count = 1000000;

	for (uint32_t i = 0; i < count; ++i)
		a.Append(i);

	EXPECT_EQ(count, a.Size());
	EXPECT_LE(count, a.Capacity());

	for (uint32_t i = 0; i < count; ++i)
		EXPECT_EQ(a[i], i);
}