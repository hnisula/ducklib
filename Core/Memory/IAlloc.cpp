#include "HeapAllocator.h"

namespace ducklib
{
IAlloc* DefAlloc()
{
	static HeapAllocator defaultHeapAllocator;

	return &defaultHeapAllocator;
}

IAlloc::IAlloc()
	: totalAllocatedSize(0)
	, allocatedSize(0)
	, allocationCount(0) {}

void* IAlloc::Allocate(uint64 size, uint8_t align)
{
	return AllocateInternal(size, align);
}

void* IAlloc::Reallocate(void* ptr, uint64 size)
{
	return ReallocateInternal(ptr, size);
}

void IAlloc::Free(void* ptr)
{
	FreeInternal(ptr);
}
}
