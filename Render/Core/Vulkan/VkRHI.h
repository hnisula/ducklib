#pragma once

#include "Lib/vulkan.h"
#include "Lib/glfw3.h"
#include "../IRHI.h"

namespace DuckLib
{
namespace Render
{
class VkRHI : IRHI
{
public:
	static IRHI* GetInstance();

	void Init();

protected:
	VkRHI();

	VkInstance instance;

	bool isInitialized;
};
}
}
