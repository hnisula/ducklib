#include "IAlloc.h"

namespace DuckLib
{
IAlloc::IAlloc()
	: totalAllocatedSize(0)
	, allocatedSize(0)
	, allocationCount(0) {}

void* IAlloc::Allocate(uint64_t size, uint8_t align)
{
	return AllocateInternal(size, align);
}

void* IAlloc::Reallocate(void* ptr, uint64_t size)
{
	return ReallocateInternal(ptr, size);
}

void IAlloc::Free(void* ptr)
{
	FreeInternal(ptr);
}
}
