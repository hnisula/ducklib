#pragma once

#include <d3d12.h>
#include "../IFrameSyncer.h"

namespace DuckLib
{
namespace Render
{
class D3D12FrameSyncer : IFrameSyncer
{
public:

	void WaitForFrame() override;

protected:

	D3D12FrameSyncer(ID3D12Fence* apiFence, uint32_t numFrameBuffers, ID3D12CommandQueue* commandQueue);
	~D3D12FrameSyncer() override;

private:

	ID3D12CommandQueue* commandQueue;
	ID3D12Fence* apiFence;
	HANDLE fenceEventHandle;
};
}
}
