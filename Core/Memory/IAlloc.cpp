#include "HeapAllocator.h"

namespace ducklib
{
IAllocator* DefAlloc()
{
	static HeapAllocator defaultHeapAllocator;

	return &defaultHeapAllocator;
}

IAllocator::IAllocator()
	: totalAllocatedSize(0)
	, allocatedSize(0)
	, allocationCount(0) {}

void* IAllocator::Allocate(uint64 size, uint8_t align)
{
	return AllocateInternal(size, align);
}

void* IAllocator::Reallocate(void* ptr, uint64 size)
{
	return ReallocateInternal(ptr, size);
}

void IAllocator::Free(void* ptr)
{
	FreeInternal(ptr);
}
}
