#pragma once
#include <vector>
#include "D3D12Adapter.h"
#include "../Resources/ImageBuffer.h"
#include "../Adapter.h"

namespace DuckLib
{
namespace Render
{
class D3D12ResourceAllocator
{
public:

	ImageBuffer* AllocatorImageBuffer();
	D3D12Adapter* AllocateAdapter();

	void FreeImageBuffer(ImageBuffer* imageBuffer);
	void FreeAdapter(D3D12Adapter* adapter);
	
private:

	std::vector<D3D12Adapter*> adapters;
};
}
}
