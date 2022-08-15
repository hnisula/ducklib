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

protected:
	VulkanSwapChain(
		uint32 width,
		uint32 height,
		Format format,
		VkSwapchainKHR vkSwapChain,
		uint32 bufferCount,
		const ImageBuffer* images,
		VkFence vkFence,
		VkDevice vkDevice);

	void* GetApiHandle() const override;

	VkSwapchainKHR vkSwapChain;
	VkDevice vkDevice;
	VkFence vkFence;
	HANDLE fenceEventHandle;
};
}
