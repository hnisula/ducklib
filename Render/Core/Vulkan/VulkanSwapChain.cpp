#include "VulkanSwapChain.h"

namespace DuckLib::Render
{
VulkanSwapChain::~VulkanSwapChain() {}
void VulkanSwapChain::Present() {}
void VulkanSwapChain::WaitForFrame() {}

VulkanSwapChain::VulkanSwapChain(
	uint32 width,
	uint32 height,
	Format format,
	VkSwapchainKHR vkSwapChain,
	uint32 bufferCount,
	const ImageBuffer* images)
{
	this->width = width;
	this->height = height;
	this->format = format;
	this->vkSwapChain = vkSwapChain;
	this->numBuffers = bufferCount;

	for (uint32 i = 0; i < bufferCount; ++i)
		this->buffers[i] = images[i];
}

void* VulkanSwapChain::GetApiHandle() const
{
	return vkSwapChain;
}
}
