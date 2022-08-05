#include "VulkanDevice.h"

#include "VulkanCommon.h"
#include "Lib/vulkan_win32.h"
#include "Lib/glfw3.h"

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
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	uint32_t surfaceFormatCount;
	uint32_t presentModeCount;
	VkSurfaceKHR surface = CreateWindowSurface(windowHandle);

	// TEST
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);

	VkSurfaceFormatKHR* surfaceFormats = alloc->Allocate<VkSurfaceFormatKHR>(surfaceFormatCount);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	VkPresentModeKHR* presentModes = alloc->Allocate<VkPresentModeKHR>(presentModeCount);

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

	alloc->Free(surfaceFormats);
	alloc->Free(presentModes);

	// TODO: fix
	return nullptr;
}

void VulkanDevice::DestroySwapChain(ISwapChain* swapChain) {}
void VulkanDevice::SignalCompletion(ISwapChain* swapChain) {}

VulkanDevice::VulkanDevice(
	VkDevice vkDevice,
	VkQueue commandQueue,
	VkPhysicalDevice physicalDevice,
	VkInstance vkInstance)
	: alloc(DefAlloc())
	, vkInstance(vkInstance)
	, physicalDevice(physicalDevice)
	, device(vkDevice)
	, commandQueue(commandQueue) {}

VkSurfaceKHR VulkanDevice::CreateWindowSurface(HWND windowHandle)
{
	VkSurfaceKHR surface;
	VkWin32SurfaceCreateInfoKHR createInfo{};
	HINSTANCE instance = GetModuleHandle(NULL);

	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hinstance = instance;
	createInfo.hwnd = windowHandle;

	DL_VK_CHECK(
		vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &surface),
		"Failed to create Vulkan window surface");

	return surface;
}
}
