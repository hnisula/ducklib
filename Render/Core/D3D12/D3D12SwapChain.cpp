#include "Lib/d3dx12.h"
#include "D3D12SwapChain.h"
#include <stdexcept>
#include "D3D12Common.h"

namespace DuckLib::Render
{
D3D12SwapChain::D3D12SwapChain(
	uint32 width,
	uint32 height,
	Format format,
	IDXGISwapChain1* apiSwapChain,
	uint32 bufferCount,
	const ImageBuffer* images,
	ID3D12Fence* apiFence,
	ID3D12DescriptorHeap* descriptorHeap,
	uint32 descriptorSize)
		: ISwapChain()
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->d3dSwapChain = apiSwapChain;
	this->numBuffers = bufferCount;
	this->d3dRenderFence = apiFence;
	this->rtDescriptorHeap = descriptorHeap;
	this->descriptorSize = descriptorSize;

	for (uint32 i = 0; i < bufferCount; ++i)
		this->buffers[i] = images[i];

	fenceEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (fenceEventHandle == NULL)
		throw std::runtime_error("Failed to create fence event for swap chain");
}

D3D12SwapChain::~D3D12SwapChain()
{
	if (d3dSwapChain)
		d3dSwapChain->Release();
}

void D3D12SwapChain::Present()
{
	DL_D3D12_CHECK(d3dSwapChain->Present(0, 0), "Failed to present");
	++currentFrameIndex;
}

void D3D12SwapChain::WaitForFrame()
{
	uint32 bufferIndex = currentFrameIndex % numBuffers;
	uint64 latestCompletedFrame = d3dRenderFence->GetCompletedValue();
	uint64 expectedFrame = frameCounters[bufferIndex];

	if (latestCompletedFrame - expectedFrame < UINT32_MAX / 2)
	{
		if (FAILED(d3dRenderFence->SetEventOnCompletion(frameCounters[bufferIndex], fenceEventHandle)))
			throw std::runtime_error("Failed to set completion event on D3D12 frame fence");
		WaitForSingleObjectEx(fenceEventHandle, INFINITE, FALSE);
	}
}

void* D3D12SwapChain::GetApiHandle() const
{
	return d3dSwapChain;
}
}
