#pragma once
#include "IAlloc.h"

namespace DuckLib
{
class HeapAllocator final : public IAlloc
{
public:
	~HeapAllocator() override;

	void* AllocateInternal(uint64_t size, uint8_t align) override;
	void* ReallocateInternal(void* ptr, uint64_t size) override;
	void FreeInternal(void* ptr) override;
};

namespace Internal::Memory
{
struct Header
{
	uint64_t totalSize;
	uint8_t align;

	static constexpr char PAD_VALUE = -1;
};

uint64_t SizeWithHeaderAndAlignment(uint64_t sizeWithoutHeader, uint8_t align);
void WriteAllocHeader(void* headerPtr, uint64_t sizeWithHeader, uint8_t align);
Header* GetHeader(void* dataPtr);
uint64_t GetAllocationSize(const Header* header);
void* GetDataPtr(Header* headerPtr);
void* NextAlign(void* ptr, uint8_t align);
}
}
