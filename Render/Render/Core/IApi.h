#pragma once

namespace DuckLib
{
namespace Render
{
class IApi
{
public:

	virtual ~IApi() {};
	
	virtual const std::vector<IAdapter*>& GetAdapters() const = 0;
	virtual ISwapChain* CreateSwapChain(
		uint32_t width,
		uint32_t height,
		Format format,
		uint32_t bufferCount,
		HWND windowHandle) = 0;
	virtual ICommandBuffer* CreateCommandBuffer() = 0;
	virtual IResourceCommandBuffer* CreateResourceCommandBuffer() = 0;

	virtual void DestroySwapChain(ISwapChain* swapChain) = 0;
	virtual void DestroyCommandBuffer(ICommandBuffer* commandBuffer) = 0;
	virtual void DestroyResourceCommandBuffer(IResourceCommandBuffer* resourceCommandBuffer) = 0;

	virtual void ExecuteCommandBuffers(
		ICommandBuffer** commandBuffers,
		uint32_t numCommandBuffers) = 0;

	virtual void WaitForPreviousFrame() = 0;
};
}
}
