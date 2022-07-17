#pragma once

#include "Lib/vulkan.h"
#include "../IRHI.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib
{
namespace Render
{
class VkRHI : IRHI
{
public:
	virtual ~VkRHI();

	static IRHI* GetInstance();

	void Init();

	const IAdapter* Adapters() const override;
	uint32_t AdapterCount() const override;

protected:
	VkRHI();

	VkInstance instance;

	bool isInitialized;
};
}
}
