#pragma once
#include "Resources/Constants.h"

namespace DuckLib::Render
{
struct FrameBufferDesc
{
	Format format;
	FrameBufferLoadOp loadOp;
	FrameBufferStoreOp storeOp;
	FrameBufferLoadOp stencilLoadOp;
	FrameBufferStoreOp stencilStoreOp;

	ImageBufferLayout initialLayout;
	ImageBufferLayout finalLayout;

	FrameBufferDesc(
		Format format,
		FrameBufferLoadOp loadOp = FrameBufferLoadOp::LOAD,
		FrameBufferStoreOp storeOp = FrameBufferStoreOp::STORE,
		FrameBufferLoadOp stencilLoadOp = FrameBufferLoadOp::LOAD,
		FrameBufferStoreOp stencilStoreOp = FrameBufferStoreOp::STORE,
		ImageBufferLayout initialLayout = ImageBufferLayout::COLOR,
		ImageBufferLayout finalLayout = ImageBufferLayout::PRESENT)
		: format(format)
		, loadOp(loadOp)
		, storeOp(storeOp)
		, stencilLoadOp(stencilLoadOp)
		, stencilStoreOp(stencilStoreOp)
		, initialLayout(initialLayout)
		, finalLayout(finalLayout) {}
};

struct FrameBufferDescRef
{
	uint32 frameBufferDescIndex;
	ImageBufferLayout imageBufferLayout;

	FrameBufferDescRef(uint32 frameBufferDescIndex, ImageBufferLayout imageBufferLayout)
		: frameBufferDescIndex(frameBufferDescIndex)
		, imageBufferLayout(imageBufferLayout) {}
};

struct SubPassDescription
{
	PipelineBindPoint pipelineBindPoint;
	FrameBufferDescRef* frameBufferDescRefs;
	uint32 frameBufferDescRefCount;

	SubPassDescription(PipelineBindPoint pipelineBindPoint, FrameBufferDescRef* frameBufferDescRefs, uint32 frameBufferDescRefCount)
		: pipelineBindPoint(pipelineBindPoint)
		, frameBufferDescRefs(frameBufferDescRefs)
		, frameBufferDescRefCount(frameBufferDescRefCount) {}
};

struct PassDescription
{
	FrameBufferDesc* frameBufferDescs;
	uint32 frameBufferDescCount;
	SubPassDescription* subPassDescs;
	uint32 subPassDescCount;

	PassDescription(
		FrameBufferDesc* frameBufferDescs,
		uint32 frameBufferDescCount,
		SubPassDescription* subPassDescs,
		uint32 subPassDescCount)
		: frameBufferDescs(frameBufferDescs)
		, frameBufferDescCount(frameBufferDescCount)
		, subPassDescs(subPassDescs)
		, subPassDescCount(subPassDescCount) {}
};
}
