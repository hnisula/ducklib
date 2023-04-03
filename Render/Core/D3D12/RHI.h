#pragma once

#include "Adapter.h"
#include "Lib/dxgi1_4.h"
#include "../IRHI.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib::Render::D3D12
{
class RHI : IRHI
{
public:
	static IRHI* GetInstance();

	~RHI() override;

	const TArray<IAdapter*>& GetAdapters() const override;

protected:
	RHI();

	void InitFactory();
	// Currently only run when creating the RHI
	void EnumerateAdapters();

	static constexpr uint32_t MAX_NUM_ADAPTERS = 16;
	static constexpr uint32_t ADAPTER_DESCRIPTION_BUFFER_SIZE = 256;

	IAlloc* alloc;

	IDXGIFactory4* factory;
#ifdef _DEBUG
	ID3D12Debug* debugInterface;
#endif

	TArray<IAdapter*> adapters;
};
}
