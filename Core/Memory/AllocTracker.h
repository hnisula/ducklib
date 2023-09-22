#pragma once

#include "../Types.h"

#ifdef DL_TRACK_ALLOCS
#include <mutex>

namespace ducklib::Internal::Memory
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
		uint64 size;
		uint32 line;
	};

	AllocTracker();

	void Track(void* ptr, uint64 size, const char* file, const char* function, uint32 line);
	void Modify(void* ptr, void* newPtr, uint64 size, const char* file, const char* function, uint32 line);
	void Remove(void* ptr);

	const Entry* GetEntries();
	uint32 GetEntryCount();
	void Clear();

protected:
	uint32 FindAlloc(const void* ptr);

	const uint32 START_CAPACITY = 16;

	std::mutex lock;
	Entry* entries;
	uint32 length;
	uint32 capacity;
};
}
#endif
