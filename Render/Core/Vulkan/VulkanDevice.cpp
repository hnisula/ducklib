#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommon.h"
#include "VulkanPass.h"
#include "VulkanSwapChain.h"
#include "Core/Memory/Containers/TArray.h"
#include "Lib/vulkan_win32.h"
#include "Lib/glfw3.h"

namespace DuckLib::Render
{
VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
}

ICommandBuffer* VulkanDevice::CreateCommandBuffer()
{
	VkCommandBuffer vkCommandBuffer;
	VkCommandBufferAllocateInfo allocInfo{};

	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	DL_VK_CHECK(vkAllocateCommandBuffers(vkDevice, &allocInfo, &vkCommandBuffer), "Failed to create Vulkan command buffer");

	VulkanCommandBuffer* cmdBuffer = alloc->Allocate<VulkanCommandBuffer>();
	new(cmdBuffer) VulkanCommandBuffer(vkCommandBuffer);

	return cmdBuffer;
}

IPass* VulkanDevice::CreatePass(PassDescription passDesc)
{
	// Setup all Vulkan descs
	TArray<VkAttachmentDescription> vkAttachmentDescs(nullptr, passDesc.frameBufferDescs.Length());

	for (uint32 i = 0; i < passDesc.frameBufferDescs.Length(); ++i)
	{
		vkAttachmentDescs[i] = {};
		vkAttachmentDescs[i].loadOp = MapToVulkanLoadOp(passDesc.frameBufferDescs[i].loadOp);
		vkAttachmentDescs[i].storeOp = MapToVulkanStoreOp(passDesc.frameBufferDescs[i].storeOp);
		vkAttachmentDescs[i].stencilStoreOp = MapToVulkanStoreOp(passDesc.frameBufferDescs[i].stencilStoreOp);
		vkAttachmentDescs[i].stencilLoadOp = MapToVulkanLoadOp(passDesc.frameBufferDescs[i].stencilLoadOp);
		vkAttachmentDescs[i].initialLayout = MapToVulkanImageLayout(passDesc.frameBufferDescs[i].initialLayout);
		vkAttachmentDescs[i].finalLayout = MapToVulkanImageLayout(passDesc.frameBufferDescs[i].finalLayout);
		vkAttachmentDescs[i].format = MapToVulkanFormat(passDesc.frameBufferDescs[i].format);
		vkAttachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
	}

	TArray<VkAttachmentReference*> vkAttachmentRefPtrs(passDesc.subPassDescs.Length());
	TArray<VkSubpassDescription> vkSubPassDescs(nullptr, passDesc.subPassDescs.Length());

	for (uint32 i = 0; i < passDesc.subPassDescs.Length(); ++i)
	{
		uint32 attachmentRefCount = passDesc.subPassDescs[i].frameBufferDescRefs.Length();
		vkAttachmentRefPtrs.Append(alloc->Allocate<VkAttachmentReference>(attachmentRefCount));
		TArray vkAttachmentRefs(vkAttachmentRefPtrs.Last(), attachmentRefCount);

		for (uint32 u = 0; u < attachmentRefCount; ++u)
		{
			vkAttachmentRefs[u].attachment = passDesc.subPassDescs[i].frameBufferDescRefs[u].frameBufferDescIndex;
			vkAttachmentRefs[u].layout = MapToVulkanImageLayout(passDesc.subPassDescs[i].frameBufferDescRefs[u].imageBufferLayout);
		}

		vkSubPassDescs[i] = {};
		vkSubPassDescs[i].pipelineBindPoint = MapToVulkanBindPoint(passDesc.subPassDescs[i].pipelineBindPoint);
		vkSubPassDescs[i].colorAttachmentCount = attachmentRefCount;
		vkSubPassDescs[i].pColorAttachments = vkAttachmentRefs.Data();
		// TODO: Add the other members, too
	}

	VkRenderPassCreateInfo createInfo{};
	VkRenderPass vkPass;

	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = vkAttachmentDescs.Length();
	createInfo.pAttachments = vkAttachmentDescs.Data();
	createInfo.subpassCount = passDesc.subPassDescs.Length();
	createInfo.pSubpasses = vkSubPassDescs.Data();

	DL_VK_CHECK(vkCreateRenderPass(vkDevice, &createInfo, nullptr, &vkPass), "Failed to create Vulkan render pass");

	// Cleanup of Vulkan descs
	for (uint32 i = 0; i < vkAttachmentRefPtrs.Length(); ++i)
		alloc->Free(vkAttachmentRefPtrs[i]);

	// Create VulkanPass
	VulkanPass* pass = alloc->Allocate<VulkanPass>();

	new (pass) VulkanPass(vkPass);

	return pass;
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
	VkFormat vkFormat = MapToVulkanFormat(format);
	VkSurfaceKHR surface = CreateWindowSurface(windowHandle);
	SwapChainSupport swapChainSupport = QuerySwapChainSupport(surface);
	uint32 surfaceFormatIndex = SelectSurfaceFormatIndex(swapChainSupport.surfaceFormats, vkFormat);
	uint32 presentModeIndex = SelectPresentModeIndex(swapChainSupport.presentModes);
	VkExtent2D extent = GetSurfaceExtent(swapChainSupport.surfaceCapabilities, width, height);

	VkSwapchainCreateInfoKHR createInfo{};

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
		vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &vkSwapChain),
		"Failed to create Vulkan swap chain");

	// Get image buffers
	uint32 imageCount = 0;
	TArray<VkImage> vkImages;

	vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, nullptr);

	if (imageCount > ISwapChain::MAX_BUFFERS)
		throw std::runtime_error("Vulkan swap chain contained too many images");

	vkImages.Resize(imageCount);
	vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, vkImages.Data());

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

		DL_VK_CHECK(vkCreateImageView(vkDevice, &createInfo, nullptr, &vkImageView), "Failed to create Vulkan image view for swap chain");

		imageBuffers[i].width = width;
		imageBuffers[i].height = height;
		imageBuffers[i].format = format;
		imageBuffers[i].apiResource = vkImages[i];
		imageBuffers[i].apiDescriptor = vkImageView;
	}

	// Create swap chain wrapper
	VulkanSwapChain* swapChain = alloc->Allocate<VulkanSwapChain>();

	new(swapChain) VulkanSwapChain(width, height, format, vkSwapChain, imageCount, imageBuffers, vkDevice, vkCommandQueue);

	return swapChain;
}

void VulkanDevice::DestroySwapChain(ISwapChain* swapChain) {}
void VulkanDevice::SignalCompletion(ISwapChain* swapChain) {}

VulkanDevice::VulkanDevice(
	VkDevice vkDevice,
	VkQueue commandQueue,
	uint32 graphicsQueueFamilyIndex,
	VkPhysicalDevice physicalDevice,
	VkInstance vkInstance)
	: alloc(DefAlloc()),
	vkInstance(vkInstance),
	physicalDevice(physicalDevice),
	vkDevice(vkDevice),
	vkCommandQueue(commandQueue),
	graphicsQueueFamilyIndex(graphicsQueueFamilyIndex)
{
	// TODO: Maybe be consistent on where things are created? This is getting out of hand...

	VkCommandPoolCreateInfo cmdPoolCreateInfo{};

	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	DL_VK_CHECK(vkCreateCommandPool(vkDevice, &cmdPoolCreateInfo, nullptr, &vkCommandPool), "Failed to create Vulkan command pool");
}

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
