#include "Lib/d3dx12.h"
#include "D3D12SwapChain.h"

#include <stdexcept>

namespace DuckLib::Render
{
D3D12SwapChain::D3D12SwapChain(
	uint32 width,
	uint32 height,
	Format format,
	IDXGISwapChain1* apiSwapChain,
	uint32 bufferCount,
	ImageBuffer* images,
	ID3D12Fence* apiFence,
	ID3D12DescriptorHeap* descriptorHeap,
	uint32 descriptorSize)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->d3dSwapChain = apiSwapChain;
	this->numBuffers = bufferCount;
	this->d3dFence = apiFence;
	this->rtDescriptorHeap = descriptorHeap;
	this->descriptorSize = descriptorSize;

	for (uint32 i = 0; i < bufferCount; ++i)
		this->buffers[i] = images[i];

	fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (fenceEventHandle == nullptr)
		throw std::runtime_error("Failed to create D3D12 frame fence event");
}

D3D12SwapChain::~D3D12SwapChain()
{
	if (d3dSwapChain)
		d3dSwapChain->Release();
}

void* D3D12SwapChain::GetApiHandle() const
{
	return d3dSwapChain;
}

void D3D12SwapChain::Present()
{
	if (d3dSwapChain->Present(0, 0) != S_OK)
	{
		throw std::runtime_error("Failed to present");
	}
}

void D3D12SwapChain::WaitForFrame()
{
	uint32 bufferIndex = currentFrameIndex % numBuffers;
	uint64 latestCompletedFrame = d3dFence->GetCompletedValue();
	uint64 expectedFrame = frameCounters[bufferIndex];

	if (latestCompletedFrame - expectedFrame < UINT32_MAX / 2)
	{
		if (FAILED(d3dFence->SetEventOnCompletion(frameCounters[bufferIndex], fenceEventHandle)))
			throw std::runtime_error("Failed to set completion event on D3D12 frame fence");
		WaitForSingleObjectEx(fenceEventHandle, INFINITE, FALSE);
	}

	++currentFrameIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetCurrentCpuDescriptorHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		(uint32)this->currentFrameIndex,
		descriptorSize);
}
}
