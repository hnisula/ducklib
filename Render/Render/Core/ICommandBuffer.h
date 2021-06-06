#pragma once
#include "Resources/ImageBuffer.h"
#include "Resources/ResourceStates.h"
#include "ISwapChain.h"

namespace DuckLib
{
namespace Render
{
class ICommandBuffer
{
protected:
	~ICommandBuffer() = default;
public:

	virtual const void* GetApiHandle() const = 0;

	virtual void Reset() = 0;
	virtual void Close() = 0;

	virtual void Transition(ImageBuffer* image, ResourceState from, ResourceState to) = 0;
	virtual void SetRT(ImageBuffer* rt) = 0;
	virtual void SetRT(ISwapChain* swapChain) = 0;

	virtual void Clear(ImageBuffer* rt, float* rgbaColor) = 0;

private:
	
};
}
}
