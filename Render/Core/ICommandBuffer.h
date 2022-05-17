#pragma once
#include "ISwapChain.h"
#include "Resources/Buffer.h"
#include "Resources/ImageBuffer.h"
#include "Resources/PrimitiveTopology.h"
#include "Resources/ResourceStates.h"
#include "Resources/ImageBuffer.h"
#include "Resources/Buffer.h"
#include "Resources/InputDescription.h"
#include "ISwapChain.h"
#include "Resources/Rect.h"
#include "Resources/Viewport.h"

namespace DuckLib
{
namespace Render
{
class ICommandBuffer
{
protected:
	~ICommandBuffer() = default;
public:

	virtual void* GetApiHandle() const = 0;

	virtual void Reset() = 0;
	virtual void Close() = 0;

	virtual void Transition(ImageBuffer* image, ResourceState from, ResourceState to) = 0;
	virtual void SetRT(ImageBuffer* rt) = 0;
	virtual void SetRT(ISwapChain* swapChain) = 0;
	virtual void SetIndexBuffer(Buffer* buffer) = 0;
	virtual void SetVertexBuffers(Buffer** buffer, uint32_t count, uint32_t startSlot) = 0;
	virtual void SetInputDeclaration(InputDescription* inputDescription) = 0;

	virtual void Clear(ImageBuffer* rt, float* rgbaColor) = 0;

	virtual void SetViewport(const Viewport& viewport) = 0;
	virtual void SetScissorRect(const Rect& rect) = 0;

	virtual void SetPrimitiveTopology(PrimitiveTopology topology) = 0;
	virtual void SetVertexBuffer(uint32_t startIndex, uint32_t numViews, Buffer** buffers) = 0;

	virtual void Draw() = 0;

private:
	
};
}
}
