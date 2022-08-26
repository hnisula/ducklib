#pragma once
#include "Lib/vulkan.h"
#include "Render/Core/IPass.h"

namespace DuckLib::Render
{
class VulkanPass : public IPass
{
public:

protected:
	VkRenderPass vkPass;
};
}
