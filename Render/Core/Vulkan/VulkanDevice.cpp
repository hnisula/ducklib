#include "VulkanDevice.h"
#include "VulkanCommon.h"
#include "VulkanFormats.h"
#include "VulkanSwapChain.h"
#include "Core/Memory/Containers/TArray.h"
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

ISwapChain* VulkanDevice::CreateSwapChain(uint32_t width, uint32_t height, Format format, uint32_t bufferCount, HWND windowHandle)
{
	// Create swap chain
	VkFormat vkFormat = ToVulkanFormat(format);
	VkSurfaceKHR surface = CreateWindowSurface(windowHandle);
	SwapChainSupport swapChainSupport = QuerySwapChainSupport(surface);
	uint32 surfaceFormatIndex = SelectSurfaceFormatIndex(swapChainSupport.surfaceFormats, vkFormat);
	uint32 presentModeIndex = SelectPresentModeIndex(swapChainSupport.presentModes);
	VkExtent2D extent = GetSurfaceExtent(swapChainSupport.surfaceCapabilities, width, height);

	VkSwapchainCreateInfoKHR createInfo {};

	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = 2;
	createInfo.imageFormat = swapChainSupport.surfaceFormats[surfaceFormatIndex].format;
	createInfo.imageColorSpace = swapChainSupport.surfaceFormats[surfaceFormatIndex].colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = swapChainSupport.presentModes[presentModeIndex];
	createInfo.clipped = VK_TRUE;

	VkSwapchainKHR vkSwapChain;

	DL_VK_CHECK(
		vkCreateSwapchainKHR(device, &createInfo, nullptr, &vkSwapChain),
		"Failed to create Vulkan swap chain");

	// Get image buffers
	uint32 imageCount = 0;
	TArray<VkImage> vkImages;

	vkGetSwapchainImagesKHR(device, vkSwapChain, &imageCount, nullptr);

	if (imageCount > ISwapChain::MAX_BUFFERS)
		throw std::runtime_error("Vulkan swap chain contained too many images");

	vkImages.Resize(imageCount);
	vkGetSwapchainImagesKHR(device, vkSwapChain, &imageCount, vkImages.Data());
	
	ImageBuffer imageBuffers[ISwapChain::MAX_BUFFERS];

	for (uint32 i = 0; i < imageCount; ++i)
	{
		VkImageView vkImageView;
		VkImageViewCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.format = vkFormat;
		createInfo.image = vkImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		DL_VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &vkImageView), "Failed to create Vulkan image view for swap chain");

		imageBuffers[i].width = width;
		imageBuffers[i].height = height;
		imageBuffers[i].format = format;
		imageBuffers[i].apiResource = vkImages[i];
		imageBuffers[i].apiDescriptor = vkImageView;
	}

	// Create fence
	VkFenceCreateInfo fenceCreateInfo{};
	VkFence vkFence;

	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;

	DL_VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &vkFence), "Failed to create Vulkan fence for swap chain");

	// Create swap chain wrapper
	VulkanSwapChain* swapChain = alloc->Allocate<VulkanSwapChain>();

	new (swapChain) VulkanSwapChain(width, height, format, vkSwapChain, imageCount, imageBuffers, vkFence, device);

	return swapChain;
}

void VulkanDevice::DestroySwapChain(ISwapChain* swapChain) {}
void VulkanDevice::SignalCompletion(ISwapChain* swapChain) {}

VulkanDevice::VulkanDevice(VkDevice vkDevice, VkQueue commandQueue, VkPhysicalDevice physicalDevice, VkInstance vkInstance)
	: alloc(DefAlloc()), vkInstance(vkInstance), physicalDevice(physicalDevice), device(vkDevice), commandQueue(commandQueue) {}

VkSurfaceKHR VulkanDevice::CreateWindowSurface(HWND windowHandle)
{
	VkSurfaceKHR surface;
	VkWin32SurfaceCreateInfoKHR createInfo{};
	HINSTANCE instance = GetModuleHandle(NULL);

	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hinstance = instance;
	createInfo.hwnd = windowHandle;

	DL_VK_CHECK(vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &surface), "Failed to create Vulkan window surface");

	return surface;
}

VulkanDevice::SwapChainSupport VulkanDevice::QuerySwapChainSupport(VkSurfaceKHR surface)
{
	SwapChainSupport support;
	uint32_t surfaceFormatCount;
	uint32_t presentModeCount;

	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
	support.surfaceFormats.Resize(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, support.surfaceFormats.Data());

	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	support.presentModes.Resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, support.presentModes.Data());

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &support.surfaceCapabilities);

	return support;
}

uint32 VulkanDevice::SelectSurfaceFormatIndex(const TArray<VkSurfaceFormatKHR>& surfaceFormats, VkFormat desiredFormat)
{
	for (uint32 i = 0; i < surfaceFormats.Length(); ++i)
		if (surfaceFormats[i].format == desiredFormat)
			return i;

	throw std::runtime_error("Failed to select Vulkan surface format");
}

// Currently hardcoded to select immediate mode (no v-sync)
uint32 VulkanDevice::SelectPresentModeIndex(const TArray<VkPresentModeKHR>& presentModes)
{
	for (uint32 i = 0; i < presentModes.Length(); ++i)
		if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			return i;

	throw std::runtime_error("Failed to find Vulkan immediate present mode");
}

VkExtent2D VulkanDevice::GetSurfaceExtent(
	const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
	uint32 width,
	uint32 height) const
{
	if (width >= surfaceCapabilities.minImageExtent.width
		&& width <= surfaceCapabilities.maxImageExtent.width
		&& height >= surfaceCapabilities.minImageExtent.height
		&& height <= surfaceCapabilities.maxImageExtent.height)
		return surfaceCapabilities.currentExtent;

	throw std::runtime_error("Failed to validate swap chain resolution with Vulkan surface");
}
}
