#pragma once
#include "Render/Core/ISwapChain.h"

namespace DuckLib::Render
{
class VulkanSwapChain : public ISwapChain
{
public:
	~VulkanSwapChain() override;

	void Present() override;
	void WaitForFrame() override;

protected:
	VulkanSwapChain();

	void* GetApiHandle() const override;
};
}
