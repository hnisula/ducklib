#pragma once
#include "Resources/Format.h"
#include "Resources/ImageBuffer.h"

namespace DuckLib::Render
{
class ISwapChain
{
public:
	static constexpr uint32 MAX_BUFFERS = 4;

	virtual ~ISwapChain() {}

	ImageBuffer* GetBuffer(uint32 index);
	ImageBuffer* GetCurrentBuffer();
	uint64 GetSignalValue();

	virtual void Present() = 0;
	virtual void WaitForFrame() = 0;

	// TODO: Remove as it seems unused so far? Usage of this probably requires knowledge of specific type, anyway
	virtual void* GetApiHandle() const = 0;

protected:
	ISwapChain();

	uint32 width;
	uint32 height;
	Format format;
	uint32 numBuffers;
	// Should these be stored in the resource allocator or individually inside these other structures?
	// There are others that don't have any obvious parent so they should be stored in the allocator. Do the same with these? Yes, probably.
	ImageBuffer buffers[MAX_BUFFERS];

	uint64 currentFrameIndex;
	uint64 frameCounters[MAX_BUFFERS];
};
}
