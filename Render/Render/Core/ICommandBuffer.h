#pragma once
#include "Resources/ImageBuffer.h"

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

	virtual void TransitionTo(ImageBuffer* image) = 0;

private:
	
};
}
}
