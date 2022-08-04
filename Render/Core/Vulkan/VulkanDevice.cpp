#include "VulkanDevice.h"

namespace DuckLib::Render
{
VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(device, nullptr);
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

ISwapChain* VulkanDevice::CreateSwapChain(
	uint32_t width,
	uint32_t height,
	Format format,
	uint32_t bufferCount,
	HWND windowHandle)
{
	// TODO: Implement
	return nullptr;
}

void VulkanDevice::DestroySwapChain(ISwapChain* swapChain) {}
void VulkanDevice::SignalCompletion(ISwapChain* swapChain) {}

VulkanDevice::VulkanDevice(VkDevice apiDevice, VkQueue commandQueue)
{
	this->device = apiDevice;
	this->commandQueue = commandQueue;
}
}
