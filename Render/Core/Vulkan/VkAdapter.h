#pragma once

#include "Lib/vulkan.h"
#include "Core/Memory/IAlloc.h"
#include "Render/Core/IAdapter.h"

namespace DuckLib::Render
{
class VkAdapter : public IAdapter
{
public:

	friend class VkRHI;

	void* GetApiHandle() const override;
	IDevice* CreateDevice() override;

private:
	VkAdapter(const char* description, bool isHardware, VkPhysicalDevice apiAdapter);
	~VkAdapter() override;

	IAlloc* alloc();

	VkPhysicalDevice apiAdapter;
};
}
