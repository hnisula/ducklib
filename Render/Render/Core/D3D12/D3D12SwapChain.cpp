#include "D3D12SwapChain.h"

namespace DuckLib
{
namespace Render
{
D3D12SwapChain::D3D12SwapChain(uint32_t width, uint32_t height, Format format, IDXGISwapChain1* apiSwapChain)
{ 
	this->width = width;
	this->height = height;
	this->format = format;
	this->apiSwapChain = apiSwapChain;
}

D3D12SwapChain::~D3D12SwapChain()
{
	if (apiSwapChain)
		apiSwapChain->Release();
}

void* D3D12SwapChain::GetApiHandle() const
{
	return apiSwapChain;
}
}
}
