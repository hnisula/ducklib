#include "IFrameSyncer.h"

namespace DuckLib
{
namespace Render
{
IFrameSyncer::IFrameSyncer(uint32_t numFrameBuffers)
	: frameIndex(0)
{
	this->numFrameBuffers = numFrameBuffers;

	for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		frameCounters[i] = 0;
}
}
}
