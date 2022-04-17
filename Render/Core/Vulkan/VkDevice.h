#pragma once
#include "../IDevice.h"

namespace DuckLib
{
namespace Render
{
class VkDevice : IDevice
{
	ICommandBuffer* CreateCommandBuffer() override;

	void DestroyCommandBuffer(ICommandBuffer* commandBuffer) override;

	void ExecuteCommandBuffers(
		ICommandBuffer** commandBuffers,
		uint32_t numCommandBuffers) override;
};
}
}
