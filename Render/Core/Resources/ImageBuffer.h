#pragma once
#include "Format.h"

namespace DuckLib::Render
{
struct ImageBuffer
{
	enum class Layout
	{
		COLOR, // DRAW?
		PRESENT,
		TRANSFER
	};

	uint32 width;
	uint32 height;
	uint32 depth;
	Format format;
	void* apiResource;
	void* apiDescriptor;
};
}
