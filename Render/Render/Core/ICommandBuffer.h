#pragma once
#include "Resources/ImageBuffer.h"
#include "Resources/ResourceStates.h"

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

private:
	
};
}
}
