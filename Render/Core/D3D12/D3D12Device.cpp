#include <exception>
#include "Lib/d3dx12.h"
#include "Core/Memory/IAlloc.h"
#include "D3D12Device.h"
#include "D3D12CommandBuffer.h"
#include "D3D12SwapChain.h"
#include "D3D12Formats.h"

namespace DuckLib::Render
{
D3D12Device::~D3D12Device()
{
	for (ISwapChain* swapChain : swapChains)
		DestroySwapChain(swapChain);
}

ISwapChain* D3D12Device::CreateSwapChain(
	uint32_t width,
	uint32_t height,
	Format format,
	uint32_t bufferCount,
	HWND windowHandle)
{
	// Create swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	IDXGISwapChain3* d3dSwapChain;
	ID3D12Resource* apiBuffer;

	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = ToD3D12Format(format);
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = 0;

	// Only IDXGISwapChain1 can be created but we can query another version of the interface afterwards
	IDXGISwapChain1* d3dSwapChain1;

	DL_D3D12_CHECK(
		d3dFactory->CreateSwapChainForHwnd(commandQueue, windowHandle, &swapChainDesc, nullptr, nullptr, &d3dSwapChain1),
		"Failed to create swap chain");

	d3dSwapChain1->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&d3dSwapChain);

	// Create image buffer descriptors
	uint32_t descriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	ID3D12DescriptorHeap* descriptorHeap = CreateDescriptorHeap(
		bufferCount,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorIterator(
		descriptorHeap->GetCPUDescriptorHandleForHeapStart());
	ImageBuffer rtvHandles[ISwapChain::MAX_BUFFERS];

	for (uint32_t i = 0; i < bufferCount; ++i)
	{
		DL_D3D12_CHECK(
			d3dSwapChain->GetBuffer(i, IID_PPV_ARGS(&apiBuffer)),
			"Failed to get buffer from D3D12 swap chain");

		d3dDevice->CreateRenderTargetView(apiBuffer, nullptr, descriptorIterator);

		rtvHandles[i].width = width;
		rtvHandles[i].height = height;
		rtvHandles[i].format = format;
		rtvHandles[i].apiResource = apiBuffer;
		rtvHandles[i].apiDescriptor = (void*)descriptorIterator.ptr;

		descriptorIterator.Offset(1, descriptorSize);
	}

	// Create fence
	ID3D12Fence* apiFence;

	DL_D3D12_CHECK(
		d3dDevice->CreateFence(UINT_MAX, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&apiFence)),
		"Failed to create fence for frame syncing");

	// Create swap chain wrapper
	D3D12SwapChain* swapChain = DefAlloc()->New<D3D12SwapChain>(
		width,
		height,
		format,
		d3dSwapChain,
		bufferCount,
		rtvHandles,
		apiFence,
		descriptorHeap,
		descriptorSize);
	swapChains.push_back(swapChain);

	return swapChain;
}

ICommandBuffer* D3D12Device::CreateCommandBuffer()
{
	ID3D12CommandAllocator* apiCommandAllocator;
	ID3D12GraphicsCommandList1* apiCommandList;

	HRESULT result = d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&apiCommandAllocator));

	if (result != S_OK)
		throw std::runtime_error("Failed to create D3D12 command allocator");

	result = d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		apiCommandAllocator,
		nullptr,
		IID_PPV_ARGS(&apiCommandList));

	if (result != S_OK)
		throw std::runtime_error("Failed to create D3D12 command list");

	apiCommandList->Close();

	return new(DefAlloc()->Allocate<D3D12CommandBuffer>()) D3D12CommandBuffer(apiCommandList, apiCommandAllocator);
}

void D3D12Device::DestroySwapChain(ISwapChain* swapChain)
{
	swapChains.erase(std::find(swapChains.begin(), swapChains.end(), swapChain));

	DefAlloc()->Delete(swapChain);
}

void D3D12Device::DestroyCommandBuffer(ICommandBuffer* commandBuffer)
{
}

void D3D12Device::ExecuteCommandBuffers(ICommandBuffer** commandBuffers, uint32_t numCommandBuffers)
{
	ID3D12CommandList* apiLists[128];

	for (uint32_t i = 0; i < numCommandBuffers; ++i)
		apiLists[i] = (ID3D12CommandList*)commandBuffers[i]->GetApiHandle();

	commandQueue->ExecuteCommandLists(numCommandBuffers, apiLists);
}

void D3D12Device::SignalCompletion(ISwapChain* swapChain)
{
	D3D12SwapChain* d3dSwapChain = (D3D12SwapChain*)swapChain;
	uint64_t signalValue = swapChain->GetSignalValue();

	if (commandQueue->Signal(d3dSwapChain->d3dRenderFence, signalValue) != S_OK)
		throw std::runtime_error("Failed to signal completion in D3D12 rendering");
}

D3D12Device::D3D12Device(ID3D12Device* d3dDevice, IDXGIFactory4* dxgiFactory)
{
	this->d3dFactory = dxgiFactory;
	this->d3dDevice = d3dDevice;
	commandQueue = CreateQueue(D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_FLAG_NONE);
}

ID3D12CommandQueue* D3D12Device::CreateQueue(
	D3D12_COMMAND_LIST_TYPE type,
	D3D12_COMMAND_QUEUE_FLAGS flags)
{
	ID3D12CommandQueue* queue;
	D3D12_COMMAND_QUEUE_DESC queueDesc{};

	queueDesc.Type = type;
	queueDesc.Flags = flags;

	HRESULT result = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue));

	if (result != S_OK)
		throw std::runtime_error("Failed to create D3D12 queue");

	return queue;
}

ID3D12DescriptorHeap* D3D12Device::CreateDescriptorHeap(
	uint32_t numDescriptors,
	D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	ID3D12DescriptorHeap* apiDescriptorHeap;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.NumDescriptors = numDescriptors;
	heapDesc.Type = type;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	DL_D3D12_CHECK(
		d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&apiDescriptorHeap)),
		"Failed to create D3D12 descriptor heap");

	return apiDescriptorHeap;
}

ImageBuffer* D3D12Device::CreateImageBuffer(
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	Format format,
	ID3D12Resource* apiResource,
	D3D12_CPU_DESCRIPTOR_HANDLE apiDescriptor)
{
	ImageBuffer* imageBuffer = DefAlloc()->New<ImageBuffer>(
		width,
		height,
		depth,
		format,
		apiResource,
		(void*)apiDescriptor.ptr);

	return imageBuffer;
}
}
