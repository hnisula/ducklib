#include "ISwapChain.h"

namespace DuckLib
{
namespace Render
{
ISwapChain::ISwapChain()
{
	currentBufferIndex = 0;
}

ImageBuffer* ISwapChain::GetBuffer(uint32_t index)
{
	return &buffers[index];
}

ImageBuffer* ISwapChain::GetCurrentBuffer()
{
	return &buffers[currentBufferIndex % bufferCount];
}

void ISwapChain::AdvanceBuffer()
{
	++currentBufferIndex;
}
}
}
