#pragma once
#include "Constants.h"

namespace DuckLib::Render
{
struct Buffer
{
	uint32 size;
	Format format;
	void* apiResource;
	void* apiDescriptor;
};
}
