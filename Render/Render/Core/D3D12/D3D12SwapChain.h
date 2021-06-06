#pragma once
#include <d3d12.h>
#include <dxgi1_2.h>
#include "../ISwapChain.h"

namespace DuckLib
{
namespace Render
{
class D3D12SwapChain : public ISwapChain
{
public:

	friend class D3D12Api;

	D3D12SwapChain(
		uint32_t width,
		uint32_t height,
		Format format,
		IDXGISwapChain1* apiSwapChain,
		uint32_t bufferCount,
		ImageBuffer* rts,
		ID3D12Fence* apiFence,
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize);	// TODO: Please change this to take factory and device and create everything here!
	~D3D12SwapChain() override;

	void* GetApiHandle() const override;
	
	void Present() override;
	void WaitForFrame() override;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCpuDescriptorHandle();

protected:

	void A(uint32_t newFrameIndex);	// TODO: Return next value to signal and put this func in interface

private:

	IDXGISwapChain1* apiSwapChain;
	ID3D12Fence* apiFence;
	ID3D12DescriptorHeap* rtDescriptorHeap;
	HANDLE fenceEventHandle;
	uint32_t descriptorSize;
};
}
}
