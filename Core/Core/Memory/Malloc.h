#pragma once
#include "IAlloc.h"

namespace DuckLib
{
class Malloc final : public IAlloc
{
public:
	~Malloc() override;

private:
	void* AllocateInternal(uint64_t size, uint8_t align) override;
	void* ReallocateInternal(void* ptr, uint64_t size) override;
	void FreeInternal(void* ptr) override;
};
}
