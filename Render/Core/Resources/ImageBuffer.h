#pragma once
#include <cstdint>
#include "Format.h"

namespace DuckLib
{
namespace Render
{
struct ImageBuffer
{
	uint32 width;
	uint32 height;
	uint32 depth;
	Format format;
	void* apiResource;
	void* apiDescriptor;
};
}
}
