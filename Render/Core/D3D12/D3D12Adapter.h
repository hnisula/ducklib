#pragma once
#include <dxgi.h>
#include "D3D12Device.h"
#include "../IAdapter.h"
#include "Core/Memory/IAlloc.h"

namespace DuckLib::Render
{
class D3D12Adapter : public IAdapter
{
public:

	friend class D3D12RHI;

	void* GetApiHandle() const override;
	IDevice* CreateDevice() override;

protected:
	D3D12Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter);
	~D3D12Adapter() override;

	IAlloc* alloc;

	IDXGIFactory4* apiFactory;
	IDXGIAdapter1* apiAdapter;

	D3D12Device* device;
};
}
