#include <stdexcept>
#include "D3D12RHI.h"
#include "D3D12Device.h"
#include "Core/Memory/IAlloc.h"

namespace DuckLib
{
namespace Render
{
IRHI* D3D12RHI::GetInstance()
{
	static D3D12RHI rhi;

	if (!rhi.isInitialized)
		rhi.Init();

	return &rhi;
}
void D3D12RHI::Init()
{
	
}

const IAdapter* D3D12RHI::Adapters() const
{
	if (!isInitialized)
		throw std::runtime_error("D3D12 RHI not initialized when trying to get adapters");

	IDXGIAdapter1* adapterIt;
	DXGI_ADAPTER_DESC1 adapterDesc;

	for (uint32_t i = 0; factory->EnumAdapters1(i, &adapterIt); ++i)
	{
		adapterIt->GetDesc1(&adapterDesc);
		char descriptionBuffer[128];
		size_t dummy;

		wcstombs_s(&dummy, descriptionBuffer, adapterDesc.Description, 128);

		// S_FALSE because apparently that's what it's supposed to do on success with null device
		if (D3D12CreateDevice(
			adapterIt,
			DL_D3D_FEATURE_LEVEL,
			_uuidof(ID3D12Device),
			nullptr) == S_FALSE)
		{
			IAdapter* adapter = DefAlloc()->New<D3D12Adapter>(
				descriptionBuffer,
				(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0,
				adapterIt);

			adapters[adapterCount++] = adapter;
			// TArray<D3D12Adapter*> adaptersArr = TArray<D3D12Adapter*>((void*)adapters, i + 1, MAX_NUM_ADAPTERS);
			// adaptersArr.Append(adapter);
		}
	}
}

uint32_t D3D12RHI::AdapterCount() const
{
	return adapterCount;
}


void D3D12RHI::EnumerateAdapters()
{
	IDXGIAdapter1* adapterIt;
	DXGI_ADAPTER_DESC1 adapterDescs[MAX_NUM_ADAPTERS];
	uint32_t newAdapterCount = 0;

	for (; factory->EnumAdapters1(newAdapterCount, &adapterIt); ++newAdapterCount)
	{
		adapterIt->GetDesc1(&adapterDescs[newAdapterCount]);

		if (D3D12CreateDevice(adapterIt, DL_D3D_FEATURE_LEVEL, _uuidof(ID3D12Device), nullptr) == S_FALSE)
		{

		}
	}
}
}
}
