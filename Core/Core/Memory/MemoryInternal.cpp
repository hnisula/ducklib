#include "MemoryInternal.h"
#include "Malloc.h"

namespace DuckLib
{
IAlloc& DefAlloc()
{
	static Malloc malloc;

	return malloc;
}

namespace Internal
{
namespace Memory
{
uint64_t SizeWithHeader( uint64_t sizeWithoutHeader, uint8_t align )
{
	return sizeWithoutHeader + sizeof( Header ) + align;
}

void WriteAllocHeader( void* headerPtr, uint64_t sizeWithHeader, uint8_t align )
{
	Header* header = (Header*)headerPtr;
	header->totalSize = sizeWithHeader;
	header->align = align;
	uint8_t* iterator = (uint8_t*)header + sizeof( Header );

	while ( (uintptr_t)iterator % align )
	{
		*iterator = Header::PAD_VALUE;
		++iterator;
	}
}

Header* GetHeader( void* dataPtr )
{
	uint8_t* iterator = (uint8_t*)dataPtr;

	while ( *(iterator - 1) == Header::PAD_VALUE )
		--iterator;

	return (Header*)(iterator - sizeof( Header ));
}

uint64_t GetTotalAllocSize( const Header* header )
{
	return header->totalSize - header->align - sizeof( Header );
}

void* GetDataPtr( Header* headerPtr )
{
	uintptr_t iterator = (uintptr_t)headerPtr + sizeof( Header );

	return NextAlign( (void*)iterator, headerPtr->align );
}

void* NextAlign( void* ptr, uint8_t align )
{
	uintptr_t typedPtr = (uintptr_t)ptr;
	uintptr_t error = typedPtr % align;

	if ( error )
		typedPtr += align - error;

	return (void*)typedPtr;
}
}
}
}