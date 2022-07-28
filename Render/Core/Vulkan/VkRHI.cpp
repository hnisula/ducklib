#include "VkRHI.h"
#include "Lib/glfw3.h"
#include <stdexcept>

namespace DuckLib::Render
{
VkRHI::~VkRHI() {}

IRHI* VkRHI::GetInstance()
{
	static VkRHI rhi;

	if (!rhi.isInitialized)
		rhi.Init();

	return &rhi;
}

void VkRHI::Init()
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

	isInitialized = true;
}

const IAdapter* VkRHI::LoadAdapters() const
{
	// TODO: Implement
	return nullptr;
}

uint32_t VkRHI::AdapterCount() const
{
	// TODO: Implement
	return 0;
}

VkRHI::VkRHI() : isInitialized(false) {}
}
