#include "VulkanDevice.h"

namespace DuckLib::Render
{
VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(apiDevice, nullptr);
}

ICommandBuffer* VulkanDevice::CreateCommandBuffer()
{
	// TODO: Implement
	return nullptr;
}

void VulkanDevice::DestroyCommandBuffer(ICommandBuffer* commandBuffer)
{
	// TODO: Implement
}
void VulkanDevice::ExecuteCommandBuffers(ICommandBuffer** commandBuffers, uint32_t numCommandBuffers)
{
	// TODO: Implement
}

VulkanDevice::VulkanDevice(VkDevice apiDevice)
{
	this->apiDevice = apiDevice;
}
}
