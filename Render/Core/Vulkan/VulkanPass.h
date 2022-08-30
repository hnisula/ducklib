#pragma once
#include "Lib/vulkan.h"
#include "Render/Core/IPass.h"

namespace DuckLib::Render
{
class VulkanPass : public IPass
{
protected:
	friend class VulkanDevice;

	VulkanPass(VkRenderPass vkPass)
		: vkPass(vkPass) {}

	VkRenderPass vkPass;
};
}
