#pragma once

#include <d3d12.h>

namespace DuckLib::Render
{
enum class PrimitiveTopology
{
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP
};
}
