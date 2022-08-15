#pragma once
#include <d3d12.h>
#include <dxgi1_2.h>
#include "../ISwapChain.h"

namespace DuckLib::Render
{
class D3D12SwapChain : public ISwapChain
{
public:
	friend class D3D12Device;

	// TODO: -> protected
	D3D12SwapChain(
		uint32 width,
		uint32 height,
		Format format,
		IDXGISwapChain1* apiSwapChain,
		uint32 bufferCount,
		ImageBuffer* images,
		ID3D12Fence* apiFence,
		ID3D12DescriptorHeap* descriptorHeap,
		uint32 descriptorSize);
	~D3D12SwapChain() override;

	void* GetApiHandle() const override;

	void Present() override;
	void WaitForFrame() override;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCpuDescriptorHandle();

protected:
	// TODO: Huh?
	void A(uint32 newFrameIndex); // TODO: Return next value to signal and put this func in interface

private:
	IDXGISwapChain1* d3dSwapChain;
	ID3D12Fence* d3dFence;
	ID3D12DescriptorHeap* rtDescriptorHeap;
	HANDLE fenceEventHandle;
	uint32 descriptorSize;
};
}
