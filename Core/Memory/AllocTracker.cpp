#ifdef DL_TRACK_ALLOCS
#include <cstdlib>
#include <cassert>
#include "AllocTracker.h"

namespace DuckLib
{
namespace Internal
{
namespace Memory
{


AllocTracker& GetAllocTracker()
{
	static AllocTracker allocTracker;
	return allocTracker;
}

AllocTracker::AllocTracker()
	: entries(nullptr)
	, length(0)
	, capacity(0)
{}

void AllocTracker::Track( void* ptr, uint64_t size, const char* file, const char* function,
	uint32_t line )
{
	lock.lock();
	
	if ( length == capacity )
	{
		capacity = capacity == 0 ? START_CAPACITY : (uint32_t)(capacity * 1.6);
		entries = (Entry*)realloc( entries, capacity * sizeof( Entry ) );
	}

	entries[length].ptr = ptr;
	entries[length].file = file;
	entries[length].function = function;
	entries[length].size = size;
	entries[length].line = line;

	++length;

	lock.unlock();
}

void AllocTracker::Modify( void* ptr, void* newPtr, uint64_t size, const char* file,
	const char* function, uint32_t line )
{
	lock.lock();
	
	uint32_t i = FindAlloc( ptr );

	entries[i].ptr = newPtr;
	entries[i].file = file;
	entries[i].function = function;
	entries[i].size = size;
	entries[i].line = line;

	lock.unlock();
}

void AllocTracker::Remove( void* ptr )
{
	lock.lock();
	
	uint32_t i = FindAlloc( ptr );
	uint32_t newLength = length - 1;

	entries[i].ptr = entries[newLength].ptr;
	entries[i].file = entries[newLength].file;
	entries[i].function = entries[newLength].function;
	entries[i].size = entries[newLength].size;
	entries[i].line = entries[newLength].line;

	length = newLength;

	lock.unlock();
}

const AllocTracker::Entry* AllocTracker::GetEntries()
{
	return entries;
}

uint32_t AllocTracker::GetEntryCount()
{
	return length;
}

void AllocTracker::Clear()
{
	length = 0;
}

uint32_t AllocTracker::FindAlloc( void* ptr )
{
	for ( uint32_t i = 0; i < length; ++i )
		if ( entries[i].ptr == ptr )
			return i;

	assert( false );

	return -1;
}
}
}
}
#endif