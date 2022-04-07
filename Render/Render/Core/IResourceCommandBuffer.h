#pragma once
#include "Resources/Buffer.h"

namespace DuckLib
{
namespace Render
{
class IResourceCommandBuffer
{
public:

	virtual Buffer* CreateBuffer() = 0;

private:
};
}
}
