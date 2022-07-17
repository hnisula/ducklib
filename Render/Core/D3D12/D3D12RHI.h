#pragma once

#include "D3D12Adapter.h"
#include "Lib/dxgi1_4.h"
#include "../IRHI.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib
{
namespace Render
{
class D3D12RHI : IRHI
{
public:
	static IRHI* GetInstance();

	void Init();

	const IAdapter* Adapters() const override;
	uint32_t AdapterCount() const override;

protected:
	D3D12RHI();

	void EnumerateAdapters();

	static constexpr uint32_t MAX_NUM_ADAPTERS = 16;

	IDXGIFactory4* factory;

	bool isInitialized;

	// Unsure if this is the preferred way (compared to using TArray)
	D3D12Adapter* adapters[MAX_NUM_ADAPTERS];
	uint32_t adapterCount;
};
};
}
