#include "VkAdapter.h"

namespace DuckLib::Render
{
void* VkAdapter::GetApiHandle() const
{
	return apiAdapter;
}

IDevice* VkAdapter::CreateDevice() { }

VkAdapter::VkAdapter(const char* description, bool isHardware, VkPhysicalDevice apiAdapter)
	: IAdapter(description, isHardware)
	, apiAdapter(apiAdapter) { }
}
