#include "Adapter.h"

#include <dxgi1_3.h>
#include "Common.h"
#include "Device.h"

namespace DuckLib::Render::D3D12
{
void* Adapter::GetApiHandle() const
{
	return apiAdapter;
}

IDevice* Adapter::CreateDevice()
{
	ID3D12Device* d3dDevice;
	
	DL_D3D12_CHECK(
		D3D12CreateDevice(apiAdapter, DL_D3D_FEATURE_LEVEL, IID_PPV_ARGS(&d3dDevice)),
		"Failed to create D3D12 device");

	Device* device = alloc->Allocate<Device>();

	new(device) Device(d3dDevice, factory);

	return device;
}

Adapter::Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter, IDXGIFactory4* factory)
	: IAdapter(description, isHardware)
	, alloc(DefAlloc())
	, factory(factory)
	, apiAdapter(apiAdapter) {}

Adapter::~Adapter()
{
	if (apiAdapter)
		apiAdapter->Release();
}
}
