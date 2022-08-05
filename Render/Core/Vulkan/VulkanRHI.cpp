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
	// TArray<const char*> extensions = GetInstanceExtensions();
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	const char** extensions = alloc->Allocate<const char*>(glfwExtensionCount + 2);

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_API_VERSION_1_3;

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = extensions;
#if _DEBUG
	constexpr const char* const validationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};
	createInfo.enabledLayerCount = 1;
	createInfo.ppEnabledLayerNames = validationLayers;
#else
	createInfo.enabledLayerCount = 0;
#endif

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create Vulkan instance");

	alloc->Free(extensions);
}

void VulkanRHI::EnumerateAdapters()
{
	uint32_t deviceCount;

	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("No Vulkan compatible adapters found");

	VkPhysicalDevice* devices = alloc->Allocate<VkPhysicalDevice>(deviceCount);

	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);

		if (PhysicalDeviceIsUsable(deviceProperties, deviceFeatures))
		{
			VulkanAdapter* adapter = alloc->Allocate<VulkanAdapter>();

			new(adapter) VulkanAdapter(
				deviceProperties.deviceName,
				deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
				devices[i],
				instance);

			adapters.Append(adapter);
		}
	}

	alloc->Free(devices);
}

bool VulkanRHI::PhysicalDeviceIsUsable(
	const VkPhysicalDeviceProperties& deviceProperties,
	const VkPhysicalDeviceFeatures& deviceFeatures)
{
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}
}
