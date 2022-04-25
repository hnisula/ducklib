#include <cstdlib>
#include "HeapAllocator.h"
#include "MemoryInternal.h"
#include "../Utility.h"

namespace DuckLib
{
using namespace Internal::Memory;

HeapAllocator::~HeapAllocator()
{
	if (totalAllocatedSize > 0 || allocatedSize > 0)
		DebugOutput("Not all allocations freed in heap allocator");
}

void* HeapAllocator::AllocateInternal(uint64_t size, uint8_t align)
{
	uint64_t totalSize = SizeWithHeaderAndAlignment(size, align);
	Header* header = (Header*)malloc(totalSize);

	WriteAllocHeader(header, totalSize, align);

	totalAllocatedSize += totalSize;
	allocatedSize += size;
	++allocationCount;

	return GetDataPtr(header);
}

void* HeapAllocator::ReallocateInternal(void* ptr, uint64_t size)
{
	Header* header = GetHeader(ptr);
	uint8_t align = header->align;
	uint64_t oldTotalSize = header->totalSize;
	uint64_t oldAllocationSize = GetAllocationSize(header);
	uint64_t newTotalSize = SizeWithHeaderAndAlignment(size, align);
	Header* newHeaderPtr = (Header*)realloc(header, newTotalSize);

	WriteAllocHeader(newHeaderPtr, newTotalSize, align);

	totalAllocatedSize += newTotalSize - oldTotalSize;
	allocatedSize += size - oldAllocationSize;

	return GetDataPtr(newHeaderPtr);
}

void HeapAllocator::FreeInternal(void* ptr)
{
	Header* header = GetHeader(ptr);
	uint64_t allocationSize = GetAllocationSize(header);

	totalAllocatedSize -= header->totalSize;
	allocatedSize -= allocationSize;
	--allocationCount;

	free(header);
}
}
