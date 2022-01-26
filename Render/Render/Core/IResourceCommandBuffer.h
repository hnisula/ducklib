#pragma once

namespace DuckLib
{
namespace Render
{
#undef DELETE

class IResourceCommandBuffer
{
public:

	

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
