#include "VulkanAdapter.h"

#include "VulkanDevice.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib::Render
{
void* VulkanAdapter::GetApiHandle() const
{
	return physicalDevice;
}

IDevice* VulkanAdapter::CreateDevice()
{
	uint32_t graphicsQueueFamilyIndex = GetGraphicsQueueFamilyIndex();
	float dummyQueuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueCreationInfo{};
	VkDeviceCreateInfo creationInfo{};
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	VkDevice vkDevice;

	queueCreationInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreationInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	queueCreationInfo.queueCount = 1;
	queueCreationInfo.pQueuePriorities = &dummyQueuePriority;

	creationInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	creationInfo.pQueueCreateInfos = &queueCreationInfo;
	creationInfo.queueCreateInfoCount = 1;
	creationInfo.pEnabledFeatures = &physicalDeviceFeatures;
	creationInfo.enabledExtensionCount = 0;
	creationInfo.enabledLayerCount = 0;

	// TODO: Implement validation layer stuff here

	if (vkCreateDevice(physicalDevice, &creationInfo, nullptr, &vkDevice) != VK_SUCCESS)
		throw std::exception("Failed to create Vulkan device");

	VkQueue commandQueue;
	VulkanDevice* device = alloc->Allocate<VulkanDevice>();

	vkGetDeviceQueue(vkDevice, graphicsQueueFamilyIndex, 0, &commandQueue);

	new(device) VulkanDevice(vkDevice, commandQueue);

	return device;
}

VulkanAdapter::VulkanAdapter(const char* description, bool isHardware, VkPhysicalDevice apiAdapter)
	: IAdapter(description, isHardware)
	, physicalDevice(apiAdapter)
{
	alloc = DefAlloc();
}

VulkanAdapter::~VulkanAdapter() {}

uint32_t VulkanAdapter::GetGraphicsQueueFamilyIndex()
{
	uint32_t familyCount;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);

	if (familyCount == 0)
		throw std::exception("No Vulkan queues found");

	TArray<VkQueueFamilyProperties> families(familyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, families.Data());

	for (uint32_t i = 0; i < familyCount; ++i)
	{
		if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			return i;
	}

	throw std::exception("No Vulkan graphics queue family found");
}
}
