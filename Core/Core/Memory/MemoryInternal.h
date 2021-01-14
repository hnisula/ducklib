#pragma once
#include <cstdint>

namespace DuckLib
{
namespace Internal
{
namespace Memory
{
struct Header
{
	uint64_t totalSize;
	uint8_t align;

	static const uint8_t PAD_VALUE = 0xff;
};

uint64_t SizeWithHeader( uint64_t sizeWithoutHeader, uint8_t align );
void WriteAllocHeader( void* headerPtr, uint64_t sizeWithHeader, uint8_t align );
Header* GetHeader( void* dataPtr );
uint64_t GetTotalAllocSize( const Header* header );
void* GetDataPtr( Header* headerPtr );
void* NextAlign( void* ptr, uint8_t align );
}
}
}