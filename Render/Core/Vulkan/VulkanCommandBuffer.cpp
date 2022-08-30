#include "VulkanCommandBuffer.h"
#include "VulkanCommon.h"

namespace DuckLib::Render
{
void* VulkanCommandBuffer::GetApiHandle() const
{
	return vkCommandBuffer;
}

void VulkanCommandBuffer::Reset()
{
	DL_VK_CHECK(vkResetCommandBuffer(vkCommandBuffer, 0), "Failed to reset Vulkan command buffer");
}

void VulkanCommandBuffer::Begin()
{
	VkCommandBufferBeginInfo beginInfo{};

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	DL_VK_CHECK(vkBeginCommandBuffer(vkCommandBuffer, &beginInfo), "Failed to begin Vulkan command buffer");
}

void VulkanCommandBuffer::BeginPass(const IPass* pass)
{
	
}

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer vkCommandBuffer)
	: vkCommandBuffer(vkCommandBuffer) {}
}
