#pragma once
#include "IAlloc.h"

namespace ducklib
{
class HeapAllocator final : public IAlloc
{
public:
	~HeapAllocator() override;

	void* AllocateInternal(uint64 size, uint8_t align) override;
	void* ReallocateInternal(void* ptr, uint64 size) override;
	void FreeInternal(void* ptr) override;
};

namespace Internal::Memory
{
struct Header
{
	uint64 totalSize;
	uint8_t align;

	static constexpr char PAD_VALUE = -1;
};

uint64 SizeWithHeaderAndAlignment(uint64 sizeWithoutHeader, uint8_t align);
void WriteAllocHeader(void* headerPtr, uint64 sizeWithHeader, uint8_t align);
Header* GetHeader(void* dataPtr);
uint64 GetAllocationSize(const Header* header);
void* GetDataPtr(Header* headerPtr);
void* NextAlign(void* ptr, uint8_t align);
}
}
