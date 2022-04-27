#include <gtest/gtest.h>
#include <Core/Memory/Containers/TArray.h>

using namespace DuckLib;

TEST(TArrayTests, Simple)
{
	TArray<uint32_t> a;

	a.Append(1);

	EXPECT_EQ(1u, a.Size());
	EXPECT_EQ(1u, a[0]);
	EXPECT_LE(1u, a.Capacity());
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

TEST(TArrayTests, Subscript)
{
	TArray<uint32_t> a;

	a.Append(100);
	a.Append(200);
	a.Append(300);

	EXPECT_EQ(300u, a[2]);
	EXPECT_EQ(200u, a[1]);
	EXPECT_EQ(100u, a[0]);
}

TEST(TArrayTests, ContainsFound)
{
	TArray<uint32_t> a;

	a.Append(2);
	a.Append(4);
	a.Append(8);
	a.Append(16);

	EXPECT_TRUE(a.Contains(2));
	EXPECT_TRUE(a.Contains(4));
	EXPECT_TRUE(a.Contains(8));
	EXPECT_TRUE(a.Contains(16));
}

TEST(TArrayTests, ContainsEmpty)
{
	TArray<uint32_t> a;

	EXPECT_FALSE(a.Contains(0));
}

TEST(TArrayTests, ContainsNotFound)
{
	TArray<uint32_t> a;

	a.Append(2);
	a.Append(4);
	a.Append(8);
	a.Append(16);

	EXPECT_FALSE(a.Contains(123));
	EXPECT_FALSE(a.Contains(0));
}