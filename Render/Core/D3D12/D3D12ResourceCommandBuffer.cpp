#include "D3D12ResourceCommandBuffer.h"

namespace DuckLib
{
namespace Render
{
D3D12ResourceCommandBuffer::D3D12ResourceCommandBuffer(ID3D12CommandList* apiCommandList,
	ID3D12CommandAllocator* apiCommandAllocator)
{
	this->apiCommandList = apiCommandList;
	this->apiCommandAllocator = apiCommandAllocator;
}

D3D12ResourceCommandBuffer::~D3D12ResourceCommandBuffer()
{
	apiCommandList->Release();
	apiCommandAllocator->Release();
}
}
}
