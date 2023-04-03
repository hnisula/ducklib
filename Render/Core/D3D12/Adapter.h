#pragma once
#include "../../../Core/Memory/IAlloc.h"
#include <dxgi.h>
#include "Device.h"
#include "../IAdapter.h"

namespace DuckLib::Render::D3D12
{
class Adapter : public IAdapter
{
public:

	friend class RHI;

	void* GetApiHandle() const override;
	IDevice* CreateDevice() override;

protected:
	Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter, IDXGIFactory4* factory);
	~Adapter() override;

	IAlloc* alloc;

	IDXGIFactory4* factory;
	IDXGIAdapter1* apiAdapter;
};
}
