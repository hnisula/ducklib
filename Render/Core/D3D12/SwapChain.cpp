#include "Lib/d3dx12.h"
#include "SwapChain.h"
#include <stdexcept>
#include "Common.h"

namespace DuckLib::Render::D3D12
{
SwapChain::SwapChain(
	uint32 width,
	uint32 height,
	Format format,
	IDXGISwapChain3* d3dSwapChain,
	uint32 bufferCount,
	const ImageBuffer* images,
	ID3D12DescriptorHeap* descriptorHeap,
	uint32 descriptorSize)
		: ISwapChain()
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->d3dSwapChain = d3dSwapChain;
	this->numBuffers = bufferCount;
	this->rtDescriptorHeap = descriptorHeap;
	this->descriptorSize = descriptorSize;

	for (uint32 i = 0; i < bufferCount; ++i)
		this->buffers[i] = images[i];

	fenceEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (fenceEventHandle == NULL)
		throw std::runtime_error("Failed to create fence event for swap chain");

	currentFrameIndex = d3dSwapChain->GetCurrentBackBufferIndex();
}

SwapChain::~SwapChain()
{
	if (d3dSwapChain)
		d3dSwapChain->Release();
}

void* SwapChain::GetImageAvailabilitySemaphore()
{
	throw std::runtime_error("Not implemented");
}

void SwapChain::Present()
{
	DL_D3D12_CHECK(d3dSwapChain->Present(0, 0), "Failed to present");
}

void SwapChain::PrepareFrame()
{
	currentFrameIndex = d3dSwapChain->GetCurrentBackBufferIndex();
}
}
