#pragma once
#include <cstdint>

namespace DuckLib
{
namespace Render
{
struct Handle
{
	uint32_t type : 6;
	uint32_t index : 25;
	// TODO: Consider adding a generation value to detect outdated handles being reused
};
}
}
