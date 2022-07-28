#include <stdexcept>
#include "D3D12RHI.h"
#include "D3D12Device.h"
#include "Core/Utility.h"
#include "Core/Memory/IAlloc.h"

namespace DuckLib::Render
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
	EnumerateAdapters();
}

D3D12RHI::~D3D12RHI()
{
	for (D3D12Adapter* adapter : adapters)
	{
		adapter->~D3D12Adapter();
		alloc->Free(adapter);
	}
}

const TArray<IAdapter*>& D3D12RHI::GetAdapters() const
{
	return adapters;
}

D3D12RHI::D3D12RHI()
	: isInitialized{ false } {}

void D3D12RHI::EnumerateAdapters()
{
	IDXGIAdapter1* adapterIt;
	uint32_t adapterCounter = 0;

	for (; factory->EnumAdapters1(adapterCounter, &adapterIt); ++adapterCounter)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		char descriptionBuffer[ADAPTER_DESCRIPTION_BUFFER_SIZE];

		adapterIt->GetDesc1(&adapterDesc);
		Utility::WideToMultiByteText(adapterDesc.Description, descriptionBuffer, ADAPTER_DESCRIPTION_BUFFER_SIZE);

		// S_FALSE when testing device (nullptr) == success
		if (D3D12CreateDevice(adapterIt, DL_D3D_FEATURE_LEVEL, _uuidof(ID3D12Device), nullptr) == S_FALSE)
		{
			D3D12Adapter* adapter = alloc->Allocate<D3D12Adapter>();

			new(adapter) D3D12Adapter(
				descriptionBuffer,
				(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0,
				adapterIt);

			adapters.Append(adapter);
		}
	}
}
}
