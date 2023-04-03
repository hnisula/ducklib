#include "ResourceAllocator.h"
#include "Core/Memory/IAlloc.h"

namespace DuckLib::Render::D3D12
{
ResourceAllocator::ResourceAllocator()
{
	alloc = DefAlloc();
}

ImageBuffer* ResourceAllocator::AllocatorImageBuffer()
{
	return nullptr;
}

Adapter* ResourceAllocator::AllocateAdapter()
{
	// Adapter* adapter = alloc->New<Adapter>();
	// Is this a good idea?
	Adapter* adapter = alloc->Allocate<Adapter>();

	adapters.Append(adapter);

	return adapter;
}

void ResourceAllocator::FreeImageBuffer(ImageBuffer* imageBuffer)
{
	
}

void ResourceAllocator::FreeAdapter(Adapter* adapter)
{
	// This is kind of verbose and annoying
	// std::vector<Adapter*>::iterator it = std::find(adapters.begin(), adapters.end(), adapter);
	//
	// if (it != adapters.end())
	// 	adapters.erase(it);

	// TODO: Lots of work to be done for render resource allocation!
	// alloc->Delete(adapter);
}
}
