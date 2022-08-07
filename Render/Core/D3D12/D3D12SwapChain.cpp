#include "D3D12SwapChain.h"
#include "D3D12Common.h"
#include "d3dx12.h"

namespace DuckLib
{
namespace Render
{
D3D12SwapChain::D3D12SwapChain(
	uint32 width,
	uint32 height,
	Format format,
	IDXGISwapChain1* apiSwapChain,
	uint32 bufferCount,
	ImageBuffer* rts,
	ID3D12Fence* apiFence,
	ID3D12DescriptorHeap* descriptorHeap,
	uint32 descriptorSize)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->apiSwapChain = apiSwapChain;
	this->numBuffers = bufferCount;
	this->apiFence = apiFence;
	this->rtDescriptorHeap = descriptorHeap;
	this->descriptorSize = descriptorSize;

	for (uint32 i = 0; i < bufferCount; ++i)
		this->buffers[i] = rts[i];

	fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (fenceEventHandle == nullptr)
		throw std::exception("Failed to create D3D12 frame fence event");
}

D3D12SwapChain::~D3D12SwapChain()
{
	if (apiSwapChain)
		apiSwapChain->Release();
}

void* D3D12SwapChain::GetApiHandle() const
{
	return apiSwapChain;
}

void D3D12SwapChain::Present()
{
	if (apiSwapChain->Present(0, 0) != S_OK)
	{
		throw std::exception("Failed to present");
	}
}

void D3D12SwapChain::WaitForFrame()
{
	uint32 bufferIndex = currentFrameIndex % numBuffers;
	uint64 latestCompletedFrame = apiFence->GetCompletedValue();
	uint64 expectedFrame = frameCounters[bufferIndex];

	if (latestCompletedFrame - expectedFrame < UINT32_MAX / 2)
	{
		if (FAILED(apiFence->SetEventOnCompletion(frameCounters[bufferIndex], fenceEventHandle)))
			throw std::exception("Failed to set completion event on D3D12 frame fence");
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
}
