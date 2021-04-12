#include "D3D12CommandBuffer.h"
#include "D3D12ResourceStates.h"

namespace DuckLib
{
namespace Render
{
D3D12CommandBuffer::D3D12CommandBuffer(
	ID3D12GraphicsCommandList1* apiCommandList,
	ID3D12CommandAllocator* apiCommandAllocator)
{
	this->apiCommandList = apiCommandList;
	this->apiCommandAllocator = apiCommandAllocator;
}

D3D12CommandBuffer::~D3D12CommandBuffer()
{
	// TODO: do stuff?
}

const void* D3D12CommandBuffer::GetApiHandle() const
{
	return apiCommandList;
}

void D3D12CommandBuffer::Reset()
{
	apiCommandAllocator->Reset();
	apiCommandList->Reset(apiCommandAllocator, nullptr);
}

void D3D12CommandBuffer::Close()
{
	apiCommandList->Close();
}

void D3D12CommandBuffer::Transition(ImageBuffer* image, ResourceState from, ResourceState to)
{
	D3D12_RESOURCE_BARRIER barrierDesc;

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.StateAfter = MapToD3D12ResourceState(to);
	barrierDesc.Transition.StateBefore = MapToD3D12ResourceState(from);
	barrierDesc.Transition.pResource = (ID3D12Resource*)image->apiResource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	apiCommandList->ResourceBarrier(1, &barrierDesc);
}
}
}
