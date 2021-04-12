#include "D3D12SwapChain.h"

namespace DuckLib
{
namespace Render
{
D3D12SwapChain::D3D12SwapChain(
	uint32_t width,
	uint32_t height,
	Format format,
	IDXGISwapChain1* apiSwapChain,
	uint32_t bufferCount,
	ImageBuffer* rts)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->apiSwapChain = apiSwapChain;
	this->bufferCount = bufferCount;

	for (uint32_t i = 0; i < bufferCount; ++i)
		this->buffers[i] = rts[i];
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

void D3D12SwapChain::Present()
{
	apiSwapChain->Present(0, 0);
}
}
}
