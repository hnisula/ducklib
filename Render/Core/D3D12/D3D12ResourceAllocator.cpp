#include "D3D12ResourceAllocator.h"
#include "Core/Memory/IAlloc.h"

namespace DuckLib
{
namespace Render
{
ImageBuffer* D3D12ResourceAllocator::AllocatorImageBuffer()
{
	return nullptr;
}

D3D12Adapter* D3D12ResourceAllocator::AllocateAdapter()
{
	D3D12Adapter* adapter = (D3D12Adapter*)DL_ALLOC(DefAlloc(), sizeof(D3D12Adapter));

	adapters.push_back(adapter);

	return adapter;
}

void D3D12ResourceAllocator::FreeImageBuffer(ImageBuffer* imageBuffer)
{
	
}

void D3D12ResourceAllocator::FreeAdapter(D3D12Adapter* adapter)
{
	// This is kind of verbose and annoying
	std::vector<D3D12Adapter*>::iterator it = std::find(adapters.begin(), adapters.end(), adapter);

	if (it != adapters.end())
		adapters.erase(it);

	DL_FREE(DefAlloc(), adapter);
}
}
}
