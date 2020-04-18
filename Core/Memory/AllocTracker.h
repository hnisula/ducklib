#pragma once

#ifdef DL_TRACK_ALLOCS
#include <cstdint>

namespace DuckLib
{
namespace Internal
{
namespace Memory
{
class AllocTracker
{
public:

	// TODO: AoS -> SoA? Even map?
	struct Entry
	{
		void* ptr;
		const char* file;
		const char* function;
		uint64_t size;
		uint32_t line;
	};

	static void Track( void* ptr, uint64_t size, const char* file, const char* function,
		uint32_t line );
	static void Modify( void* ptr, void* newPtr, uint64_t size, const char* file,
		const char* function, uint32_t line );
	static void Remove( void* ptr );

	static const Entry* GetEntries();
	static uint32_t GetEntryCount();
	static void Clear();

protected:

	static uint32_t FindAlloc( void* ptr );

	static const uint32_t START_CAPACITY = 16;

	static Entry* entries;
	static uint32_t length;
	static uint32_t capacity;
};
}
}
}
#endif