#pragma once
#include <cstdint>

namespace DuckLib
{
namespace Render
{
struct Buffer
{
	uint32_t something;
	uint8_t* data;
	void* apiResource;
};
}
}
