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

	friend class D3D12Device;

	const void* GetApiHandle() const override;

	void Reset() override;
	void Close() override;

	void Transition(ImageBuffer* image, ResourceState from, ResourceState to) override;
	void SetRT(ImageBuffer* rt) override;
	void SetRT(ISwapChain* swapChain) override;

	void Clear(ImageBuffer* rt, float* rgbaColor) override;

	void SetViewport(const Viewport& viewport) override;
	void SetScissorRect(const Rect& rect) override;

	void SetPrimitiveTopology(PrimitiveTopology topology) override;
	void SetVertexBuffer(uint32_t startIndex, uint32_t numViews, Buffer** buffers) override;

	void Draw() override;

protected:

	D3D12CommandBuffer(
		ID3D12GraphicsCommandList1* apiCommandList,
		ID3D12CommandAllocator* apiCommandAllocator);
	~D3D12CommandBuffer();

private:

	ID3D12GraphicsCommandList1* apiCommandList;
	ID3D12CommandAllocator* apiCommandAllocator;
};
}
}