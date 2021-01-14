#pragma once
#include <cstdint>
#include "../Format.h"

namespace DuckLib
{
namespace Render
{
struct ImageBuffer
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	Format format;
	void* apiResource;
	void* apiDescriptor;
};
}
}
