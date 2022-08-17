#include <stdexcept>
#include "ISwapChain.h"

namespace DuckLib::Render
{
ImageBuffer* ISwapChain::GetBuffer(uint32 index)
{
	return &buffers[index];
}

ImageBuffer* ISwapChain::GetCurrentBuffer()
{
	return &buffers[currentFrameIndex % numBuffers];
}

uint64 ISwapChain::GetSignalValue()
{
	uint32 bufferIndex = currentFrameIndex % numBuffers;

	frameCounters[bufferIndex] = currentFrameIndex;

	return (uint32)frameCounters[bufferIndex];
}

ISwapChain::ISwapChain()
	: currentFrameIndex(0)
{
	for (uint32 i = 0; i < MAX_BUFFERS; ++i)
		frameCounters[i] = UINT32_MAX;
}
}
