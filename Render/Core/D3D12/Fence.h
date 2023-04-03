#pragma once
#include "Core/Types.h"
#include "Lib/d3d12.h"
#include "Render/Core/IFence.h"

namespace DuckLib::Render::D3D12
{
class Fence : public IFence
{
public:
	~Fence() override = default;

	void Wait() override;

protected:
	friend class Device;

	Fence(ID3D12Fence* d3dFence);

	ID3D12Fence* d3dFence;
	HANDLE fenceEventHandle;
	uint64 expectedValue;
};
}
