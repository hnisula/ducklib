#pragma once

#include "D3D12Adapter.h"
#include "Lib/dxgi1_4.h"
#include "../IRHI.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib::Render
{
class D3D12RHI : IRHI
{
public:
	static IRHI* GetInstance();

	void Init();

	~D3D12RHI() override;
	const TArray<IAdapter*>& GetAdapters() const override;

protected:
	D3D12RHI();

	// Only run when creating the RHI
	void EnumerateAdapters();

	static constexpr uint32_t MAX_NUM_ADAPTERS = 16;
	static constexpr uint32_t ADAPTER_DESCRIPTION_BUFFER_SIZE = 256;

	IAlloc* alloc;
	IDXGIFactory4* factory;
	bool isInitialized;
	TArray<IAdapter*> adapters;
};
}
