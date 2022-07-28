#include "VulkanRHI.h"
#include "Lib/glfw3.h"
#include <stdexcept>
#include "VulkanAdapter.h"

namespace DuckLib::Render
{
VulkanRHI::~VulkanRHI() {}

const TArray<IAdapter*>& VulkanRHI::GetAdapters() const
{
	return adapters;
}

IRHI* VulkanRHI::GetInstance()
{
	static VulkanRHI rhi;

	return &rhi;
}

VulkanRHI::VulkanRHI()
	: alloc(nullptr)
	, instance(nullptr)
{
	alloc = DefAlloc();

	CreateInstance();
	EnumerateAdapters();
}

void VulkanRHI::CreateInstance()
{
	VkApplicationInfo appInfo{};
	VkInstanceCreateInfo createInfo{};
	uint32_t numExtensions = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&numExtensions);

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_3;

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = numExtensions;
	createInfo.ppEnabledExtensionNames = extensions;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance");
}

void VulkanRHI::EnumerateAdapters()
{
	uint32_t deviceCount;

	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::exception("No Vulkan compatible adapters found");

	VkPhysicalDevice* devices = alloc->Allocate<VkPhysicalDevice>(deviceCount);

	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);

		VulkanAdapter* adapter = alloc->Allocate<VulkanAdapter>();

		new(adapter) VulkanAdapter(
			deviceProperties.deviceName,
			deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
			devices[i]);

		adapters.Append(adapter);
	}

	alloc->Free(devices);
}
}
