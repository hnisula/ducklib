#pragma once
#include <d3d12.h>

#include "../IResourceCommandBuffer.h"

namespace DuckLib::Render::D3D12
{
class ResourceCommandBuffer : public IResourceCommandBuffer
{
public:
	friend class Device;

	Buffer* CreateBuffer() override;

protected:
	ResourceCommandBuffer(
		ID3D12Device* apiDevice,
		ID3D12CommandList* apiCommandList,
		ID3D12CommandAllocator* apiCommandAllocator);
	~ResourceCommandBuffer() override;

	ID3D12Device* apiDevice;
	ID3D12CommandList* apiCommandList;
	ID3D12CommandAllocator* apiCommandAllocator;
};
}
