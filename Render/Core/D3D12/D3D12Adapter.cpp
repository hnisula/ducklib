#include "D3D12Adapter.h"

namespace DuckLib
{
namespace Render
{
D3D12Adapter::D3D12Adapter(const char* description, bool isHardware, IDXGIAdapter1* apiAdapter)
	: IAdapter(description, isHardware)
	, apiAdapter(apiAdapter)
{}

void* D3D12Adapter::GetApiHandle() const
{
	return apiAdapter;
}

D3D12Adapter::~D3D12Adapter()
{
	if (apiAdapter)
		apiAdapter->Release();
}
}
}
