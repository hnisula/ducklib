#pragma once
#include "Resources/Buffer.h"

namespace DuckLib
{
namespace Render
{
#undef DELETE

class IResourceCommandBuffer
{
public:
	virtual ~IResourceCommandBuffer() {}

	virtual Buffer* CreateBuffer() = 0;

	

private:
	enum class OperationType
	{
		CREATE,
		DELETE
	};

	struct QueuedResource
	{
		void* data;
		OperationType type;
	};

	// TODO: Again read through Our Machinery's simple C containers. Should something simple be used here over std::vector?
};
}
}
