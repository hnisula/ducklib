#pragma once
#include <cstdint>

namespace DuckLib
{
namespace Render
{
class IFrameSyncer
{
public:

	IFrameSyncer(uint32_t numFrameBuffers);
	virtual ~IFrameSyncer() {}

	virtual void WaitForFrame() = 0;

protected:

	static const uint32_t MAX_FRAMES = 4;

	uint32_t numFrameBuffers;
	uint32_t frameIndex;
	uint32_t frameCounters[MAX_FRAMES];
};
}
}
