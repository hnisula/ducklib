#pragma once
#include "Core/Memory/Containers/TArray.h"
#include "Resources/ImageBuffer.h"

namespace DuckLib::Render
{
struct FrameBufferDesc
{
	Format format;
	FrameBufferLoadOp loadOp = FrameBufferLoadOp::LOAD;
	FrameBufferStoreOp storeOp = FrameBufferStoreOp::STORE;
	FrameBufferLoadOp stencilLoadOp = FrameBufferLoadOp::LOAD;
	FrameBufferStoreOp stencilStoreOp = FrameBufferStoreOp::STORE;

	ImageBufferLayout initialLayout = ImageBufferLayout::COLOR;
	ImageBufferLayout finalLayout = ImageBufferLayout::PRESENT;
};

struct FrameBufferDescRef
{
	uint32 frameBufferDescIndex;
	ImageBufferLayout imageBufferLayout;
};

struct SubPassDescription
{
	PipelineBindPoint pipelineBindPoint;
	TArray<FrameBufferDescRef> frameBufferDescRefs;
};

struct PassDescription
{
	TArray<FrameBufferDesc> frameBufferDescs;
	TArray<SubPassDescription> subPassDescs;
};

class IPass
{
public:
	virtual ~IPass() = 0;
};
}
