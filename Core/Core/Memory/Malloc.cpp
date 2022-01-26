#include <cstdlib>
#include "Malloc.h"
#include "MemoryInternal.h"

namespace DuckLib
{
using namespace Internal::Memory;

Malloc::~Malloc() {}

void* Malloc::AllocateInternal(uint64_t size, uint8_t align)
{
	uint64_t sizeWithHeader = SizeWithHeader(size, align);
	Header* headerPtr = (Header*)malloc(sizeWithHeader);

	WriteAllocHeader(headerPtr, sizeWithHeader, align);

	return GetDataPtr(headerPtr);
}

void* Malloc::ReallocateInternal(void* ptr, uint64_t size)
{
	Header* headerPtr = GetHeader(ptr);
	uint8_t align = headerPtr->align;
	uint64_t newSizeWithHeader = SizeWithHeader(size, align);
	Header* newHeaderPtr = (Header*)realloc(headerPtr, newSizeWithHeader);

	WriteAllocHeader(newHeaderPtr, newSizeWithHeader, align);

	return GetDataPtr(newHeaderPtr);
}

void Malloc::FreeInternal(void* ptr)
{
	void* headerPtr = GetHeader(ptr);

	free(headerPtr);
}
}
