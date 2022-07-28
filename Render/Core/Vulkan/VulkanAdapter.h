#pragma once

#include "Lib/vulkan.h"
#include "Core/Memory/IAlloc.h"
#include "Render/Core/IAdapter.h"

namespace DuckLib::Render
{
class VulkanAdapter : public IAdapter
{
public:

	friend class VulkanRHI;

	void* GetApiHandle() const override;
	IDevice* CreateDevice() override;

private:
	VulkanAdapter(const char* description, bool isHardware, VkPhysicalDevice apiAdapter);
	~VulkanAdapter() override;

	uint32_t GetGraphicsQueueFamilyIndex();

	IAlloc* alloc;

	VkPhysicalDevice physicalDevice;
};
}
