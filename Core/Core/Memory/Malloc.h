#pragma once
#include "IAlloc.h"

namespace DuckLib
{
class Malloc final : public IAlloc
{
public:
	~Malloc() override;

	void* Allocate( uint64_t size, uint8_t align = DEFAULT_ALIGN ) override;
	void* Reallocate( void* ptr, uint64_t size ) override;
	void Free( void* ptr ) override;

	void* Allocate( uint64_t size, uint8_t align, const char* file,
		const char* function, uint32_t line ) override;
	void* Reallocate( void* ptr, uint64_t size, const char* file,
		const char* function, uint32_t line ) override;

private:

	void TrackCreate( void* ptr, uint64_t size, const char* file, const char* function,
		uint32_t line ) const;
	void TrackModify( void* ptr, void* newPtr, uint64_t size, const char* file,
		const char* function, uint32_t line ) const;
	void TrackDelete( void* ptr ) const;
};
}