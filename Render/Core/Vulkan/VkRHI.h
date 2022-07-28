#pragma once

#include "Lib/vulkan.h"
#include "../IRHI.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib::Render
{
class VkRHI : IRHI
{
public:
	~VkRHI() override;

	static IRHI* GetInstance();

	void Init();

	const IAdapter* LoadAdapters() const override;
	uint32_t AdapterCount() const override;

protected:
	VkRHI();

	VkInstance instance;

	bool isInitialized;
};
}
