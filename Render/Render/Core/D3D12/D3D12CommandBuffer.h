#pragma once
#include <d3d12.h>
#include "../ICommandBuffer.h"

namespace DuckLib
{
namespace Render
{
class ISwapChain;

class D3D12CommandBuffer : public ICommandBuffer
{
public:

	friend class D3D12Api;

	const void* GetApiHandle() const override;

	void Reset() override;
	void Close() override;

	void Transition(ImageBuffer* image, ResourceState from, ResourceState to) override;
	void SetRT(ImageBuffer* rt) override;
	void SetRT(ISwapChain* swapChain) override;
	void SetIndexBuffer(Buffer* buffer) override;
	void SetVertexBuffers(Buffer** buffer, uint32_t count, uint32_t startSlot) override;
	void SetInputDeclaration(InputDescription* inputDescription) override;

	void Clear(ImageBuffer* rt, float* rgbaColor) override;

protected:

	D3D12CommandBuffer(
		ID3D12GraphicsCommandList1* apiCommandList,
		ID3D12CommandAllocator* apiCommandAllocator);
	~D3D12CommandBuffer();

private:

	static const uint32_t MAX_SET_VB_COUNT = 32;

	ID3D12GraphicsCommandList1* apiCommandList;
	ID3D12CommandAllocator* apiCommandAllocator;
};
}
}
