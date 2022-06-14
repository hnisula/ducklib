#pragma once

#include <d3d12.h>
#include <exception>

#define DL_D3D12_THROW_FAIL(statement, text) \
	if ((statement) != S_OK) \
		throw std::exception((text))
