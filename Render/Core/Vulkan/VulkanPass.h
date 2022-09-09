#pragma once
#include "Lib/vulkan.h"
#include "Render/Core/IPass.h"

namespace DuckLib::Render
{
class VulkanPass : public IPass
{
public:
	friend class VulkanDevice;
	friend class VulkanCommandBuffer;

	~VulkanPass() override
	{
		vkDestroyRenderPass(vkDevice, vkPass, nullptr);
	}

protected:
	VulkanPass(VkRenderPass vkPass, VkDevice vkDevice)
		: vkDevice(vkDevice), vkPass(vkPass) {}

	VkDevice vkDevice;
	VkRenderPass vkPass;
	TArray<VkFramebuffer> vkFrameBuffers;
};
}
