#pragma once
#include "../Resources/ImageBuffer.h"

namespace DuckLib
{
namespace Render
{
class D3D12ResourceAllocator
{
public:

	// Buffer* AllocateBuffer();
	ImageBuffer* AllocatorImageBuffer();

	void FreeImageBuffer(ImageBuffer* imageBuffer);
	
private:

	
};
}
}
