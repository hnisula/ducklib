#pragma once
#include "D3D12FrameSyncer.h"

#include <cassert>

namespace DuckLib
{
namespace Render
{
void D3D12FrameSyncer::WaitForFrame()
{
	uint32_t newFrameValue = frameIndex;
	uint32_t bufferIndex = frameIndex % numFrameBuffers;
	
	commandQueue->Signal(apiFence, newFrameValue);

	if (apiFence->GetCompletedValue() < frameCounters[bufferIndex])
	{
		apiFence->SetEventOnCompletion(frameCounters[bufferIndex], fenceEventHandle);
		WaitForSingleObjectEx(fenceEventHandle, INFINITE, FALSE);
	}

	frameCounters[bufferIndex] = newFrameValue;
}

D3D12FrameSyncer::D3D12FrameSyncer(ID3D12Fence* apiFence, uint32_t numFrameBuffers, ID3D12CommandQueue* commandQueue)
	: IFrameSyncer(numFrameBuffers)
{
	this->commandQueue = commandQueue;
	this->apiFence = apiFence;
}

D3D12FrameSyncer::~D3D12FrameSyncer()
{
	assert(apiFence);
	apiFence->Release();
	// CloseHandle(fenceEventHandle);	// TODO: Any way to check if it's being used?
}
}
}
