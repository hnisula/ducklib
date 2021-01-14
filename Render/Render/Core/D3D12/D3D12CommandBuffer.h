#pragma once
#include <d3d12.h>
#include "../ICommandBuffer.h"

namespace DuckLib
{
namespace Render
{
class D3D12CommandBuffer : public ICommandBuffer
{
public:

	friend class D3D12Api;

	const void* GetApiHandle() const override;

	void TransitionTo(ImageBuffer* image) override;

protected:

	D3D12CommandBuffer(ID3D12GraphicsCommandList1* apiCommandList,
		ID3D12CommandAllocator* apiCommandAllocator);
	~D3D12CommandBuffer();

private:

	ID3D12GraphicsCommandList1* apiCommandList;
	ID3D12CommandAllocator* apiCommandAllocator;
};
}
}
