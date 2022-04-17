#include "IAlloc.h"

namespace DuckLib
{
void* IAlloc::Allocate(uint64_t size, uint8_t align, const std::source_location& sourceLocation)
{
	void* ptr = AllocateInternal(size, align);

#ifdef DL_TRACK_ALLOCS
	Internal::Memory::GetAllocTracker().Track(
		ptr,
		size,
		sourceLocation.file_name(),
		sourceLocation.function_name(),
		sourceLocation.line());
#endif

	return ptr;
}

void* IAlloc::Reallocate(void* ptr, uint64_t size, const std::source_location& sourceLocation)
{
	void* newPtr = ReallocateInternal(ptr, size);

#ifdef DL_TRACK_ALLOCS
	Internal::Memory::GetAllocTracker().Modify(
		ptr,
		newPtr,
		size,
		sourceLocation.file_name(),
		sourceLocation.function_name(),
		sourceLocation.line());
#endif

	return newPtr;
}

void IAlloc::Free(void* ptr)
{
	FreeInternal(ptr);

#ifdef DL_TRACK_ALLOCS
	Internal::Memory::GetAllocTracker().Remove(ptr);
#endif
}
}
