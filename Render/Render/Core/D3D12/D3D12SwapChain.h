#pragma once
#include <dxgi1_2.h>
#include "../ISwapChain.h"

namespace DuckLib
{
namespace Render
{
class D3D12SwapChain : public ISwapChain
{
public:

	D3D12SwapChain(
		uint32_t width,
		uint32_t height,
		Format format,
		IDXGISwapChain1* apiSwapChain,
		uint32_t bufferCount,
		ImageBuffer* rts);
	~D3D12SwapChain() override;

	void* GetApiHandle() const override;

	void Present() override;

private:

	IDXGISwapChain1* apiSwapChain;
};
}
}
