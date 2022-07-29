#include "D3D12Adapter.h"

#include <dxgi1_3.h>
#include "D3D12Common.h"
#include "D3D12Device.h"

namespace DuckLib::Render
{
void* D3D12Adapter::GetApiHandle() const
{
	return apiAdapter;
}

IDevice* D3D12Adapter::CreateDevice()
{
	ID3D12Device* apiDevice;
	
	DL_D3D12_CHECK(
		D3D12CreateDevice(apiAdapter, DL_D3D_FEATURE_LEVEL, IID_PPV_ARGS(&apiDevice)),
		"Failed to create D3D12 device");

	D3D12Device* device = alloc->Allocate<D3D12Device>();

	new(device) D3D12Device(apiDevice);

	return device;
}

D3D12Adapter::D3D12Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter)
	: IAdapter(description, isHardware)
	, apiAdapter(apiAdapter) {}

D3D12Adapter::~D3D12Adapter()
{
	if (apiAdapter)
		apiAdapter->Release();
}
}
