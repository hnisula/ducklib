#pragma once
#include "Constants.h"

namespace DuckLib::Render
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
