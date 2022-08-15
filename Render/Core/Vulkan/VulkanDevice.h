#pragma once
#include "Lib/vulkan.h"
#include "../IDevice.h"
#include "Core/Memory/IAlloc.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib::Render
{
class VulkanDevice : IDevice
{
public:
	friend class VulkanAdapter;

	~VulkanDevice() override;

	ICommandBuffer* CreateCommandBuffer() override;

	void DestroyCommandBuffer(ICommandBuffer* commandBuffer) override;

	void ExecuteCommandBuffers(
		ICommandBuffer** commandBuffers,
		uint32_t numCommandBuffers) override;

	ISwapChain* CreateSwapChain(uint32_t width, uint32_t height, Format format, uint32_t bufferCount,HWND windowHandle) override;
	void DestroySwapChain(ISwapChain* swapChain) override;
	void SignalCompletion(ISwapChain* swapChain) override;

protected:
	VulkanDevice(VkDevice vkDevice, VkQueue commandQueue, VkPhysicalDevice physicalDevice, VkInstance vkInstance);

	VkSurfaceKHR CreateWindowSurface(HWND windowHandle);

	struct SwapChainSupport
	{
		TArray<VkSurfaceFormatKHR> surfaceFormats;
		TArray<VkPresentModeKHR> presentModes;
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
	};

	SwapChainSupport QuerySwapChainSupport(VkSurfaceKHR surface);
	uint32 SelectSurfaceFormatIndex(const TArray<VkSurfaceFormatKHR>& surfaceFormats, VkFormat desiredFormat);
	uint32 SelectPresentModeIndex(const TArray<VkPresentModeKHR>& presentModes);
	VkExtent2D GetSurfaceExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, uint32 width, uint32 height) const;

	IAlloc* alloc;

	VkInstance vkInstance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue commandQueue;
};
}
