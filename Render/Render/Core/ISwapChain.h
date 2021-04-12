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

	virtual void Present() = 0;
	// TODO: Rename
	void AdvanceBuffer();

protected:

	virtual void* GetApiHandle() const = 0;
	
	uint32_t width;
	uint32_t height;
	Format format;
	uint32_t bufferCount;
	// Should these be stored in the resource allocator or individually inside these other structures?
	// There are others that don't have any obvious parent so they should be stored in the allocator. Do the same with these? Yes, probably.
	ImageBuffer buffers[MAX_BUFFERS];

	uint32_t currentBufferIndex;
};
}
}
