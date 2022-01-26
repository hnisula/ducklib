#include "gtest/gtest.h"
#include <Core/Memory/Malloc.h>

class Foo
{
public:

	Foo()
	{
		v1 = 16;
		v2 = 69;
	}

	~Foo()
	{
		*p = 32;
	}

	uint32_t v1;
	uint32_t v2;
	uint32_t* p;
};

TEST(AllocMacroTest, NewDelete)
{
	DuckLib::Malloc alloc;
	uint32_t v = 2;
	Foo* foo = DL_NEW(alloc, Foo);
	foo->p = &v;

	EXPECT_EQ(16, foo->v1);
	EXPECT_EQ(69, foo->v2);

	DL_DELETE(alloc, foo);

	EXPECT_EQ(32, v);
}

TEST(AllocMacroTest, NewDeleteArray)
{
	DuckLib::Malloc alloc;
	uint32_t v = 2;
	const uint32_t arraySize = 32;
	Foo* foo = DL_NEW_ARRAY(alloc, Foo, arraySize);

	for (uint32_t i = 0; i < arraySize; ++i)
		foo[i].p = &v;

	for (uint32_t i = 0; i < arraySize; ++i)
	{
		EXPECT_EQ(16, foo->v1);
		EXPECT_EQ(69, foo->v2);
	}

	DL_DELETE_ARRAY(alloc, foo);

	EXPECT_EQ(32, v);
}
