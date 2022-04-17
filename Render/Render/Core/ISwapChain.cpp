#include "ISwapChain.h"

namespace DuckLib
{
namespace Render
{
ISwapChain::ISwapChain()
{
	currentFrameIndex = 0;

	for (uint32_t i = 0; i < MAX_BUFFERS; ++i)
		frameCounters[i] = UINT32_MAX;
}

ImageBuffer* ISwapChain::GetBuffer(uint32_t index)
{
	return &buffers[index];
}

ImageBuffer* ISwapChain::GetCurrentBuffer()
{
	return &buffers[currentFrameIndex % numBuffers];
}

uint64_t ISwapChain::GetSignalValue()
{
	uint32_t bufferIndex = currentFrameIndex % numBuffers;

	frameCounters[bufferIndex] = currentFrameIndex;

	return (uint32_t)frameCounters[bufferIndex];
}
}
}
