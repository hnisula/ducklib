#pragma once
#include <cstdint>
#include "Resources/Format.h"
#include "Resources/ImageBuffer.h"

namespace DuckLib
{
namespace Render
{
class ISwapChain
{
public:

	static const uint32_t MAX_BUFFERS = 4;

	ISwapChain();
	virtual ~ISwapChain() {}

	ImageBuffer* GetBuffer(uint32_t index);
	ImageBuffer* GetCurrentBuffer();
	uint32_t GetSignalValue();

	virtual void Present() = 0;
	virtual void WaitForFrame() = 0;

protected:

	virtual void* GetApiHandle() const = 0;
	
	uint32_t width;
	uint32_t height;
	Format format;
	uint32_t numBuffers;
	// Should these be stored in the resource allocator or individually inside these other structures?
	// There are others that don't have any obvious parent so they should be stored in the allocator. Do the same with these? Yes, probably.
	ImageBuffer buffers[MAX_BUFFERS];

	uint64_t currentFrameIndex;
	uint64_t frameCounters[MAX_BUFFERS];
};
}
}
