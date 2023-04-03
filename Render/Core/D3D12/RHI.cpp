#include <stdexcept>
#include "RHI.h"
#include "Common.h"
#include "Device.h"
#include "Core/Utility.h"
#include "Core/Memory/IAlloc.h"
#include "Core/Memory/Containers/Iterators.h"

namespace DuckLib::Render::D3D12
{
IRHI* RHI::GetInstance()
{
	static RHI rhi;

	return &rhi;
}

RHI::~RHI()
{
	for (IAdapter* adapter : adapters)
	{
		((Adapter*)adapter)->~Adapter();
		alloc->Free(adapter);
	}
}

const TArray<IAdapter*>& RHI::GetAdapters() const
{
	return adapters;
}

RHI::RHI()
	: alloc(nullptr)
	, factory(nullptr)
{
	alloc = DefAlloc();
	InitFactory();
	EnumerateAdapters();
}

void RHI::InitFactory()
{
#ifdef _DEBUG
	DL_D3D12_CHECK(
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)),
		"Failed to create DXGI factory");

	DL_D3D12_CHECK(
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)),
		"Failed to get debugInterface interface");

	debugInterface->EnableDebugLayer();
#else
	DL_D3D12_CHECK(
		CreateDXGIFactory2(IID_PPV_ARGS(&d3dFactory)),
		"Failed to create DXGI factory");
#endif
}

void RHI::EnumerateAdapters()
{
	IDXGIAdapter1* adapterIt;
	uint32_t adapterCounter = 0;

	for (; factory->EnumAdapters1(adapterCounter, &adapterIt) != DXGI_ERROR_NOT_FOUND; ++adapterCounter)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		char descriptionBuffer[ADAPTER_DESCRIPTION_BUFFER_SIZE];

		adapterIt->GetDesc1(&adapterDesc);
		Utility::WideToMultiByteText(adapterDesc.Description, descriptionBuffer, ADAPTER_DESCRIPTION_BUFFER_SIZE);

		// S_FALSE when testing device (nullptr) == success
		if (D3D12CreateDevice(adapterIt, DL_D3D_FEATURE_LEVEL, _uuidof(ID3D12Device), nullptr) == S_FALSE)
		{
			Adapter* adapter = alloc->Allocate<Adapter>();

			new(adapter) Adapter(
				descriptionBuffer,
				(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0,
				adapterIt,
				factory);

			adapters.Append(adapter);
		}
	}
}
}
