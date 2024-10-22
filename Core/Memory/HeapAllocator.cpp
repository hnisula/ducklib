#include <cstdlib>
#include "HeapAllocator.h"
#include "../Utility.h"

namespace ducklib
{
using namespace Internal::Memory;

HeapAllocator::~HeapAllocator()
{
	if (totalAllocatedSize > 0 || allocatedSize > 0)
		Utility::DebugOutput("Not all allocations freed in heap allocator");
}

void* HeapAllocator::AllocateInternal(uint64 size, uint8_t align)
{
	uint64 totalSize = SizeWithHeaderAndAlignment(size, align);
	Header* header = (Header*)malloc(totalSize);

	WriteAllocHeader(header, totalSize, align);

	totalAllocatedSize += totalSize;
	allocatedSize += size;
	++allocationCount;

	return GetDataPtr(header);
}

void* HeapAllocator::ReallocateInternal(void* ptr, uint64 size)
{
	Header* header = GetHeader(ptr);
	uint8_t align = header->align;
	uint64 oldTotalSize = header->totalSize;
	uint64 oldAllocationSize = GetAllocationSize(header);
	uint64 newTotalSize = SizeWithHeaderAndAlignment(size, align);

	header = (Header*)realloc(header, newTotalSize);
	WriteAllocHeader(header, newTotalSize, align);

	totalAllocatedSize += newTotalSize - oldTotalSize;
	allocatedSize += size - oldAllocationSize;

	return GetDataPtr(header);
}

void HeapAllocator::FreeInternal(void* ptr)
{
	Header* header = GetHeader(ptr);
	uint64 allocationSize = GetAllocationSize(header);

	totalAllocatedSize -= header->totalSize;
	allocatedSize -= allocationSize;
	--allocationCount;

	free(header);
}

namespace Internal::Memory
{
uint64 SizeWithHeaderAndAlignment(uint64 sizeWithoutHeader, uint8_t align)
{
	return sizeWithoutHeader + sizeof(Header) + align;
}

void WriteAllocHeader(void* headerPtr, uint64 sizeWithHeader, uint8_t align)
{
	Header* header = (Header*)headerPtr;
	char* iterator = (char*)header + sizeof(Header);

	header->totalSize = sizeWithHeader;
	header->align = align;

	while ((uintptr_t)iterator % align)
	{
		*iterator = Header::PAD_VALUE;
		++iterator;
	}
}

Header* GetHeader(void* dataPtr)
{
	char* iterator = (char*)dataPtr;

	while (*(iterator - 1) == Header::PAD_VALUE)
		--iterator;

	return (Header*)(iterator - sizeof(Header));
}

uint64 GetAllocationSize(const Header* header)
{
	return header->totalSize - header->align - sizeof(Header);
}

void* GetDataPtr(Header* headerPtr)
{
	void* ptrStart = (char*)headerPtr + sizeof(Header);

	return NextAlign(ptrStart, headerPtr->align);
}

void* NextAlign(void* ptr, uint8_t align)
{
	uintptr_t alignError = (uintptr_t)ptr % align;
	uintptr_t offset = alignError ? align - alignError : 0;

	return (char*)ptr + offset;
}
}
}
