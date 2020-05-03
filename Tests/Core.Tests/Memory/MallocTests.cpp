#include <gtest/gtest.h>
#include "Memory/Malloc.h"
#include "Memory/MemoryInternal.h"

using namespace DuckLib;

// TODO: Check if they've added support for x64 in the ASan support
// Because ASan support for x64 is not added yet and the values checked for are only set in debug
#ifndef NDEBUG
TEST( MallocTest, AllocCheckBlockValues )
{
	Malloc malloc;
	const uint32_t ALLOC_SIZE = 36;
	const uint8_t UNINITIALIZED_HEAP_VALUE = 0xcd;
	const uint8_t HEAP_GUARD_BYTES_VALUE = 0xfd;
	uint8_t* ptr = (uint8_t*)malloc.Allocate( ALLOC_SIZE );

	// TODO: Find a better way to either handle aligns' extra memory allocation or testing it
	for ( uint32_t i = 0; i < ALLOC_SIZE + 4; ++i )
		EXPECT_EQ( UNINITIALIZED_HEAP_VALUE, ptr[i] );

	EXPECT_EQ( HEAP_GUARD_BYTES_VALUE, ptr[ALLOC_SIZE + 4] );
}
#endif

TEST( MallocTest, AllocAndCheckHeader )
{
	Malloc malloc;
	void* ptr = malloc.Allocate( 16 );
	Internal::Memory::Header* header = Internal::Memory::GetHeader( ptr );
	uint64_t expectedAllocSize = 16 + sizeof( Internal::Memory::Header ) + IAlloc::DEFAULT_ALIGN;

	EXPECT_EQ( expectedAllocSize, header->totalSize );
	EXPECT_EQ( IAlloc::DEFAULT_ALIGN, header->align );
}