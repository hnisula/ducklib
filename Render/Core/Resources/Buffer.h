#pragma once
#include <cstdint>

namespace DuckLib
{
namespace Render
{
struct Buffer
{
	uint32 something;
	uint8_t* data;
	void* apiResource;
};
}
}
