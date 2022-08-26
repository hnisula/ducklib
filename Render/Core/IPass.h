#pragma once
#include "Resources/ImageBuffer.h"

namespace DuckLib::Render
{
struct PassDescription
{
	struct RtDesc
	{
		enum class LoadOp
		{
			LOAD,
			CLEAR,
			DONT_CARE
		};

		enum class StoreOp
		{
			STORE,
			DONT_CARE
		};

		Format format;
		LoadOp loadOp = LoadOp::LOAD;
		StoreOp storeOp = StoreOp::STORE;
		LoadOp stencilLoadOp = LoadOp::LOAD;
		StoreOp stencilStoreOp = StoreOp::STORE;

		ImageBuffer::Layout initialLayout = ImageBuffer::Layout::COLOR;
		ImageBuffer::Layout finalLayout = ImageBuffer::Layout::PRESENT;
	};

	ImageBuffer** imageBuffers;
	uint32 imageBufferCount;
};

class IPass
{
public:
};
}
