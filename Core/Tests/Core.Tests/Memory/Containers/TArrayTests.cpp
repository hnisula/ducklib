#include <gtest/gtest.h>
#include "Core/Memory/Containers/TArray.h"

using namespace DuckLib;

TEST(TArrayTests, Simple)
{
	TArray<uint32_t> a;

	a.Append(1);

	EXPECT_EQ(1u, a.Length());
	EXPECT_EQ(1u, a[0]);
	EXPECT_LE(1u, a.Capacity());
}

TEST(TArrayTests, MultipleSizeIncreases)
{
	TArray<uint32_t> a;
	constexpr uint32_t count = 1000000;

	for (uint32_t i = 0; i < count; ++i)
		a.Append(i);

	EXPECT_EQ(count, a.Length());
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

TEST(TArrayTests, CopyConstructor)
{
	TArray<uint32_t> a;

	a.Append(2);
	a.Append(4);
	a.Append(8);
	a.Append(16);

	TArray b(a);

	EXPECT_EQ(4, b.Length());
	EXPECT_LE((uint32_t)4, b.Capacity());
	EXPECT_EQ(2, b[0]);
	EXPECT_EQ(4, b[1]);
	EXPECT_EQ(8, b[2]);
	EXPECT_EQ(16, b[3]);
}

TEST(TArrayTests, CopyConstructorEmpty)
{
	TArray<uint32_t> a;
	TArray b(a);

	EXPECT_EQ(0, b.Length());
}

TEST(TArrayTests, MoveConstructor)
{
	TArray<uint32_t> a;

	a.Append(2);
	a.Append(4);
	a.Append(8);

	TArray b(std::move(a));

	EXPECT_EQ(3, b.Length());
	EXPECT_LE((uint32_t)3, b.Capacity());
	EXPECT_EQ(2, b[0]);
	EXPECT_EQ(4, b[1]);
	EXPECT_EQ(8, b[2]);
}

TEST(TArrayTests, MoveConstructorEmpty)
{
	TArray<uint32_t> a;
	TArray b(a);

	EXPECT_EQ(0, b.Length());
	EXPECT_LE((uint32_t)0, b.Capacity());
}

TEST(TArrayTests, ConstructorCopyBasicArray)
{
	uint32_t n[] = { 2, 4, 8 };
	TArray a(n, 3);

	EXPECT_EQ(3, a.Length());
	EXPECT_LE((uint32_t)3, a.Capacity());
	EXPECT_EQ(2, a[0]);
	EXPECT_EQ(4, a[1]);
	EXPECT_EQ(8, a[2]);
}

TEST(TArrayTests, ConstructorCopyBasicArrayCapacity)
{
	uint32_t n[] = { 2, 4, 8 };
	TArray a(n, 3, 12);

	EXPECT_EQ(3, a.Length());
	EXPECT_LE((uint32_t)12, a.Capacity());
	EXPECT_EQ(2, a[0]);
	EXPECT_EQ(4, a[1]);
	EXPECT_EQ(8, a[2]);
}

TEST(TArrayTests, AttachSimple)
{
	uint32_t n[] = { 2, 4, 8, 0, 0, 0 };
	TArray a = TArray<uint32_t>::Attach(n, 6);

	EXPECT_EQ(6, a.Length());
	EXPECT_LE((uint32_t)6, a.Capacity());
	EXPECT_EQ(2, a[0]);
	EXPECT_EQ(4, a[1]);
	EXPECT_EQ(8, a[2]);
	EXPECT_EQ(0, a[3]);
	EXPECT_EQ(0, a[4]);
	EXPECT_EQ(0, a[5]);
}

TEST(TArrayTests, AttachSimpleSubset)
{
	uint32_t n[] = {2, 4, 8, 0, 0, 0};
	TArray a = TArray<uint32_t>::Attach(n, 3, 6);

	EXPECT_EQ(3, a.Length());
	EXPECT_LE((uint32_t)6, a.Capacity());

	a.Append(16);
	a.Append(32);
	a.Append(64);

	EXPECT_EQ(2, a[0]);
	EXPECT_EQ(4, a[1]);
	EXPECT_EQ(8, a[2]);
	EXPECT_EQ(16, a[3]);
	EXPECT_EQ(32, a[4]);
	EXPECT_EQ(64, a[5]);

	EXPECT_ANY_THROW(a.Append(128));
}

TEST(TArrayTests, AttachWithAlloc)
{
	uint32_t* n = DefAlloc()->Allocate<uint32_t>(4);

	n[0] = 2;
	n[1] = 4;
	n[2] = 8;
	n[3] = 16;

	TArray a = TArray<uint32_t>::Attach(n, 4, DefAlloc());

	EXPECT_EQ(4, a.Length());
	EXPECT_LE((uint32_t)4, a.Capacity());
	EXPECT_EQ(2, a[0]);
	EXPECT_EQ(4, a[1]);
	EXPECT_EQ(8, a[2]);
	EXPECT_EQ(16, a[3]);

	a.Append(32);
	a.Append(64);
	a.Append(128);

	EXPECT_EQ(7, a.Length());
	EXPECT_LE((uint32_t)7, a.Capacity());
	EXPECT_EQ(32, a[4]);
	EXPECT_EQ(64, a[5]);
	EXPECT_EQ(128, a[6]);
}
