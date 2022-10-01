#include "D3D12Fence.h"
#include <stdexcept>

namespace DuckLib::Render
{
D3D12Fence::~D3D12Fence() {}

void D3D12Fence::Wait()
{
	if (d3dFence->GetCompletedValue() - expectedValue < UINT32_MAX / 2)
	{
		if (FAILED(d3dFence->SetEventOnCompletion(expectedValue++, fenceEventHandle)))
			throw std::runtime_error("Failed to set completion event on D3D12 frame fence");

		WaitForSingleObjectEx(fenceEventHandle, INFINITE, FALSE);
	}
}

D3D12Fence::D3D12Fence(ID3D12Fence* d3dFence) : d3dFence(d3dFence), expectedValue(1) {}
}
