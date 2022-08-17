#pragma once
#include <Windows.h>
#include "Lib/vulkan.h"
#include "Render/Core/ISwapChain.h"

namespace DuckLib::Render
{
class VulkanSwapChain : public ISwapChain
{
public:
	friend class VulkanDevice;

	~VulkanSwapChain() override;

	void Present() override;
	void WaitForFrame() override;

	void* GetApiHandle() const override;

protected:
	VulkanSwapChain(
		uint32 width,
		uint32 height,
		Format format,
		VkSwapchainKHR vkSwapChain,
		uint32 bufferCount,
		const ImageBuffer* images,
		VkDevice vkDevice,
		VkQueue vkPresentQueue);

	VkSwapchainKHR vkSwapChain;
	VkDevice vkDevice;
	VkQueue vkPresentQueue;
	VkFence vkRenderFence;
	VkSemaphore vkRenderFinishedSemaphore;
	VkSemaphore vkImageAvailableSemaphore;
};
}
