#pragma once
#include <dxgi.h>
#include "D3D12Device.h"
#include "../IAdapter.h"

namespace DuckLib::Render
{
class D3D12Adapter : public IAdapter
{
public:

	// TODO: Reconsider this whole split of allocation and creation!
	friend class D3D12ResourceAllocator;

	friend class D3D12RHI;

	void* GetApiHandle() const override;

	IDevice* CreateDevice() override;

protected:
	D3D12Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter);
	~D3D12Adapter() override;

	IDXGIFactory4* apiFactory;
#ifdef _DEBUG
	ID3D12Debug* debugInterface;
#endif
	IDXGIAdapter1* apiAdapter;

	D3D12Device* device;
};
}
