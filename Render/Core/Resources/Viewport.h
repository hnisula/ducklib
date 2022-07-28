#pragma once

namespace DuckLib::Render
{
struct Viewport
{
	float topLeftX;
	float topleftY;
	float width;
	float height;
	float minDepth;
	float maxDepth;
};
}
