#include "ResourceCommandBuffer.h"

namespace DuckLib::Render::D3D12
{
Buffer* ResourceCommandBuffer::CreateBuffer()
{
	// apiDevice->CreateCommittedResource()

	// TODO: Implement
	return nullptr;
}

ResourceCommandBuffer::ResourceCommandBuffer(
	ID3D12Device* apiDevice,
	ID3D12CommandList* apiCommandList,
	ID3D12CommandAllocator* apiCommandAllocator)
{
	this->apiDevice = apiDevice;
	this->apiCommandList = apiCommandList;
	this->apiCommandAllocator = apiCommandAllocator;
}

ResourceCommandBuffer::~ResourceCommandBuffer()
{
	apiCommandList->Release();
	apiCommandAllocator->Release();
}
}
