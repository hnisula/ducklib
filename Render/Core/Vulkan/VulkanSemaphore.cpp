#include "VulkanSemaphore.h"

namespace DuckLib::Render
{
VulkanSemaphore::VulkanSemaphore(VkSemaphore vkSemaphore, VkDevice vkDevice)
	: vkSemaphore(vkSemaphore)
	, vkDevice(vkDevice) {}

VulkanSemaphore::~VulkanSemaphore()
{
	vkDestroySemaphore(vkDevice, vkSemaphore, nullptr);
}
}
