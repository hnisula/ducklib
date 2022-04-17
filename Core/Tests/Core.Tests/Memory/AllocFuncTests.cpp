#include "gtest/gtest.h"
#include <Core/Memory/Malloc.h>

using namespace DuckLib;

class Foo
{
public:
	Foo()
		: v1(16)
		, v2(69)
	{}

	~Foo()
	{
		*p = 32;
	}

	uint32_t v1;
	uint32_t v2;
	uint32_t* p;
};

TEST(AllocFuncTest, New)
{
	char* t = New<char>();
	*t = 'a';
	int d = 43;
}
