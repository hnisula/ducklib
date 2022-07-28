#pragma once

#include <d3d12.h>
#include <exception>
#include "../Resources/PrimitiveTopology.h"

#define DL_D3D12_CHECK(statement, text) \
	if ((statement) != S_OK) \
		throw std::exception((text))

namespace DuckLib::Render
{
inline D3D12_PRIMITIVE_TOPOLOGY MapPrimitiveTopology(PrimitiveTopology topology)
{
	switch (topology)
	{
	case PrimitiveTopology::POINT_LIST: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PrimitiveTopology::LINE_LIST: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveTopology::LINE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PrimitiveTopology::TRIANGLE_LIST: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveTopology::TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default: return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}
}
