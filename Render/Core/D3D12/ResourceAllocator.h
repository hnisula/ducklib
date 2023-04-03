#pragma once
#include "Adapter.h"
#include "../Resources/ImageBuffer.h"
#include "Core/Memory/Containers/TArray.h"

namespace DuckLib::Render::D3D12
{
class ResourceAllocator
{
public:

	ResourceAllocator();

	ImageBuffer* AllocatorImageBuffer();
	Adapter* AllocateAdapter();

	void FreeImageBuffer(ImageBuffer* imageBuffer);
	void FreeAdapter(Adapter* adapter);
	
private:

	IAlloc* alloc;

	TArray<Adapter*> adapters;
};
}
