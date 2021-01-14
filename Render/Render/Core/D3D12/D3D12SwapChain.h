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

	D3D12SwapChain(uint32_t width, uint32_t height, Format format, IDXGISwapChain1* apiSwapChain);
	~D3D12SwapChain();

	void* GetApiHandle() const override;
	
private:

	IDXGISwapChain1* apiSwapChain;
};
}
}
