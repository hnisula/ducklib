#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_2.h>
#include <vector>
#include "D3D12Adapter.h"
#include "D3D12SwapChain.h"
#include "../ICommandBuffer.h"
#include "../IResourceCommandBuffer.h"
#include "../IApi.h"
#include "../Format.h"

namespace DuckLib
{
namespace Render
{
#define DL_D3D12_THROW_FAIL(statement, text) \
	if ((statement) != S_OK) \
		throw std::exception((text))

const D3D_FEATURE_LEVEL DL_D3D_FEATURE_LEVEL = D3D_FEATURE_LEVEL_12_1;

class D3D12Api : public IApi
{
public:

	D3D12Api();
	~D3D12Api() override;

	const IAdapter* const* GetAdapters() const override;

	ISwapChain* CreateSwapChain(
		uint32_t width,
		uint32_t height,
		Format format,
		uint32_t bufferCount,
		HWND windowHandle) override;
	ICommandBuffer* CreateCommandBuffer() override;
	IResourceCommandBuffer* CreateResourceCommandBuffer() override;

	void DestroySwapChain(ISwapChain* swapChain) override;
	void DestroyCommandBuffer(ICommandBuffer* commandBuffer) override;
	void DestroyResourceCommandBuffer(IResourceCommandBuffer* resourceCommandBuffer) override;

	void ExecuteCommandBuffers(ICommandBuffer** commandBuffers, uint32_t numCommandBuffers) override;

private:

	static DXGI_FORMAT MapFormat(Format format);

	void EnumAndCreateAdapters();
	ID3D12CommandQueue* CreateQueue(D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_FLAGS flags);
	ID3D12DescriptorHeap* CreateDescriptorHeap(uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
	ImageBuffer* CreateImageBuffer(
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		Format format,
		ID3D12Resource* apiResource,
		D3D12_CPU_DESCRIPTOR_HANDLE apiDescriptor);

	void DestroyQueue(ID3D12CommandQueue* queue);
	void DestroyDescriptorHeap(ID3D12DescriptorHeap* descriptorHeap);

	void DestroyAdapters();

	IDXGIFactory2* factory;
	ID3D12Device* device;

	ID3D12CommandQueue* commandQueue;

	ID3D12DescriptorHeap* descriptorHeap;

	std::vector<IAdapter*> adapters;
	std::vector<ISwapChain*> swapChains;
};
}
}
