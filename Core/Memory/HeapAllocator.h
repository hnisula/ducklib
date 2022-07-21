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
}
