#pragma once
#include <cstdint>
#include "../Format.h"
#include "Resources/ImageBuffer.h"

namespace DuckLib
{
namespace Render
{
class ISwapChain
{
public:

	static const uint32_t MAX_BUFFERS = 4;

protected:

	virtual ~ISwapChain() = 0;

	virtual void* GetApiHandle() const = 0;
	
	uint32_t width;
	uint32_t height;
	Format format;
	uint32_t bufferCount;
	ImageBuffer* buffers[MAX_BUFFERS];
};
}
}
