#pragma once

#ifdef DL_TRACK_ALLOCS
#include <mutex>
#include <cstdint>

namespace DuckLib::Internal::Memory
{
class AllocTracker;

AllocTracker& GetAllocTracker();

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

	AllocTracker();

	void Track( void* ptr, uint64_t size, const char* file, const char* function,
		uint32_t line );
	void Modify( void* ptr, void* newPtr, uint64_t size, const char* file,
		const char* function, uint32_t line );
	void Remove( void* ptr );

	const Entry* GetEntries();
	uint32_t GetEntryCount();
	void Clear();

protected:

	uint32_t FindAlloc( void* ptr );

	const uint32_t START_CAPACITY = 16;

	std::mutex lock;
	Entry* entries;
	uint32_t length;
	uint32_t capacity;
};
}
#endif
