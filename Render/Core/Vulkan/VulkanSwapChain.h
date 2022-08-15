#pragma once
#include "Lib/vulkan.h"
#include "Render/Core/ISwapChain.h"

namespace DuckLib::Render
{
class VulkanSwapChain : public ISwapChain
{
public:
	friend class VulkanDevice;

	~VulkanSwapChain() override;

	void Present() override;
	void WaitForFrame() override;

protected:
	VulkanSwapChain(
		uint32 width,
		uint32 height,
		Format format,
		VkSwapchainKHR vkSwapChain,
		uint32 bufferCount,
		const ImageBuffer* images);
		// ID3D12Fence* apiFence,
		// ID3D12DescriptorHeap* descriptorHeap,
		// uint32 descriptorSize);

	void* GetApiHandle() const override;

	VkSwapchainKHR vkSwapChain;
};
}
