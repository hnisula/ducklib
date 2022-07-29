#pragma once
#include "Lib/vulkan.h"
#include "../IDevice.h"
#include "Core/Memory/IAlloc.h"

namespace DuckLib::Render
{
class VulkanDevice : IDevice
{
public:
	friend class VulkanAdapter;

	~VulkanDevice() override;

	ICommandBuffer* CreateCommandBuffer() override;

	void DestroyCommandBuffer(ICommandBuffer* commandBuffer) override;

	void ExecuteCommandBuffers(
		ICommandBuffer** commandBuffers,
		uint32_t numCommandBuffers) override;

	ISwapChain* CreateSwapChain(
		uint32_t width,
		uint32_t height,
		Format format,
		uint32_t bufferCount,
		HWND windowHandle) override;
	void DestroySwapChain(ISwapChain* swapChain) override;
	void SignalCompletion(ISwapChain* swapChain) override;

protected:
	VulkanDevice(VkDevice apiDevice, VkQueue commandQueue);

	IAlloc* alloc;

	VkDevice device;
	VkQueue commandQueue;
};
}
