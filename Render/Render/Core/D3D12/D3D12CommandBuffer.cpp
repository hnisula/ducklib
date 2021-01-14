#include "D3D12CommandBuffer.h"

namespace DuckLib
{
namespace Render
{
const void* D3D12CommandBuffer::GetApiHandle() const
{
	return apiCommandList;
}

void D3D12CommandBuffer::TransitionTo(ImageBuffer* image)
{
	// apiCommandList->ResourceBarrier(1, image->)
}

D3D12CommandBuffer::D3D12CommandBuffer(ID3D12GraphicsCommandList1* apiCommandList,
	ID3D12CommandAllocator* apiCommandAllocator)
{
	this->apiCommandList = apiCommandList;
	this->apiCommandAllocator = apiCommandAllocator;
}

D3D12CommandBuffer::~D3D12CommandBuffer()
{
	if (apiCommandList)
		apiCommandList->Release();
}
}
}
