#pragma once
#include <dxgi.h>
#include "../IAdapter.h"

namespace DuckLib
{
namespace Render
{
class D3D12Adapter : public IAdapter
{
public:

	// TODO: Reconsider this whole split of allocation and creation!
	// This does not work, as it is IAlloc::Delete() that does the deletion!
	friend class D3D12ResourceAllocator;

	D3D12Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter);

	void* GetApiHandle() const override;

protected:

	~D3D12Adapter() override;

	IDXGIAdapter1* apiAdapter;
};
}
}
