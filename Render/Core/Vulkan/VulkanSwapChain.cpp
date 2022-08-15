#include "VulkanSwapChain.h"
#include <stdexcept>

namespace DuckLib::Render
{
VulkanSwapChain::~VulkanSwapChain()
{
	for (uint32 i = 0; i < numBuffers; ++i)
		vkDestroyImageView(vkDevice, (VkImageView)buffers[i].apiDescriptor, nullptr);
}
void VulkanSwapChain::Present() {}
void VulkanSwapChain::WaitForFrame() {}

VulkanSwapChain::VulkanSwapChain(
	uint32 width,
	uint32 height,
	Format format,
	VkSwapchainKHR vkSwapChain,
	uint32 bufferCount,
	const ImageBuffer* images,
	VkFence vkFence,
	VkDevice vkDevice)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->vkSwapChain = vkSwapChain;
	this->numBuffers = bufferCount;
	this->vkFence = vkFence;
	this->vkDevice = vkDevice;

	for (uint32 i = 0; i < bufferCount; ++i)
		this->buffers[i] = images[i];

	fenceEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (fenceEventHandle == NULL)
		throw std::runtime_error("Failed to win32 fence event for Vulkan swap chain");
}

void* VulkanSwapChain::GetApiHandle() const
{
	return vkSwapChain;
}
}
