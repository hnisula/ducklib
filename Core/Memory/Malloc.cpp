#include <cstdlib>
#include "Malloc.h"
#include "MemoryInternal.h"

#ifdef DL_TRACK_ALLOCS
#include "AllocTracker.h"
#endif

namespace DuckLib
{
using namespace Internal::Memory;

Malloc::~Malloc()
{}

void* Malloc::Allocate( uint64_t size, uint8_t align )
{
	uint64_t sizeWithHeader = SizeWithHeader( size, align );
	Header* headerPtr = (Header*)malloc( (size_t)sizeWithHeader );
	WriteAllocHeader( headerPtr, sizeWithHeader, align );
	void* dataPtr = GetDataPtr( headerPtr );

#ifdef DL_TRACK_ALLOCS
	// std::experimental::source_location callLocation;	// TODO: Cry
#endif

	return dataPtr;
}

void* Malloc::Reallocate( void* ptr, uint64_t size )
{
	Header* headerPtr = GetHeader( ptr );
	uint8_t align = headerPtr->align;
	uint64_t newSizeWithHeader = SizeWithHeader( size, align );
	Header* newHeaderPtr = (Header*)realloc( headerPtr, (size_t)newSizeWithHeader );
	WriteAllocHeader( newHeaderPtr, newSizeWithHeader, align );

	return GetDataPtr( newHeaderPtr );
}

void Malloc::Free( void* ptr )
{
	void* headerPtr = GetHeader( ptr );
	free( headerPtr );

#ifdef DL_TRACK_ALLOCS
	AllocTracker::Remove( ptr );
#endif
}

#ifdef DL_TRACK_ALLOCS
void* Malloc::Allocate( uint64_t size, uint8_t align, const char* file, const char* function,
	uint32_t line )
{
	void* ptr = Allocate( size, align );
	AllocTracker::Track( ptr, size, file, function, line );

	return ptr;
}

void* Malloc::Reallocate( void* ptr, uint64_t size, const char* file, const char* function,
	uint32_t line )
{
	void* newPtr = Reallocate( ptr, size );
	AllocTracker::Modify( ptr, newPtr, size, file, function, line );

	return ptr;
}
#endif
}