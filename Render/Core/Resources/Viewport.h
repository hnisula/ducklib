#pragma once

namespace DuckLib::Render
{
struct Viewport
{
	float topLeftX;
	float topLeftY;
	float width;
	float height;
	float minDepth;
	float maxDepth;
};
}
