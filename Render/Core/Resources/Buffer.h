#pragma once
#include <cstdint>
#include "Format.h"

namespace DuckLib::Render
{
struct Buffer
{
	uint32_t size;
	Format format;
	void* apiResource;
	void* apiDescriptor;
};
}
