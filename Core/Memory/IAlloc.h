#pragma once
#include <new>
#include "MemoryInternal.h"

namespace DuckLib
{
class IAlloc;

IAlloc& DefAlloc();

class IAlloc
{
public:
	virtual ~IAlloc() {};

	virtual void* Allocate(uint64_t size, uint8_t align = DEFAULT_ALIGN) = 0;
	virtual void* Reallocate(void* ptr, uint64_t size) = 0;
	virtual void Free(void* ptr) = 0;

	// Ugly stuff that can, hopefully, be removed soon (std::source_location)
	virtual void* Allocate(uint64_t size,
		uint8_t align,
		const char* file,
		const char* function,
		uint32_t line) = 0;
	virtual void* Reallocate(void* ptr,
		uint64_t size,
		const char* file,
		const char* function,
		uint32_t line) = 0;

	static const uint8_t DEFAULT_ALIGN = 4;
};

namespace Internal
{
namespace Memory
{
#ifdef DL_TRACK_ALLOCS
template <typename T>
T* NewArray(IAlloc& alloc, uint32_t size, const char* file, const char* function, uint32_t line)
{
	T* ptr = (T*)alloc.Allocate(sizeof(T) * size, alignof(T), file, function, line);

	for (uint32_t i = 0; i < size; ++i)
		new(&ptr[i]) T();

	return ptr;
}
#else
template < typename T >
T* NewArray(IAlloc& alloc, uint32_t size)
{
	T* ptr = (T*)alloc.Allocate(sizeof(T) * size, alignof(T));

	for (uint32_t i = 0; i < size; ++i)
		new (&ptr[i]) T();

	return ptr;
}
#endif

template <typename T>
void Delete(IAlloc& alloc, T* ptr)
{
	ptr->~T();
	alloc.Free(ptr);
}

template <typename T>
void DeleteArray(IAlloc& alloc, T* ptr)
{
	Header* header = GetHeader(ptr);
	uint64_t arrayLength = GetTotalAllocSize(header) / sizeof(T);

	for (uint64_t i = 0; i < arrayLength; ++i)
		ptr[i].~T();

	alloc.Free(ptr);
}
}
}
}

// While std::source_location (C++20:Library Fundamentals, Version 2) is not supported, we need this
#ifdef DL_TRACK_ALLOCS
#define DL_NEW(alloc, type, ...) \
	new ((alloc).Allocate(sizeof(type), alignof(type), __FILE__, __FUNCTION__, __LINE__)) type{__VA_ARGS__}

#define DL_NEW_ARRAY(alloc, type, size) \
	(::DuckLib::Internal::Memory::NewArray<type>((alloc), (size), __FILE__, __FUNCTION__, __LINE__))

#define DL_ALLOC(alloc, size) \
	((alloc).Allocate(size, IAlloc::DEFAULT_ALIGN, __FILE__, __FUNCTION__, __LINE__))

#define DL_ALLOC_A(alloc, size, align) \
	((alloc).Allocate(size, align, __FILE__, __FUNCTION__, __LINE__))

#define DL_REALLOC(alloc, ptr, size) \
	((alloc).Reallocate(ptr, size, __FILE__, __FUNCTION__, __LINE__))
#else
#define DL_NEW(alloc, type, ...) \
	new ((alloc).Allocate(sizeof(type), alignof(type))) type{__VA_ARGS__}

#define DL_NEW_ARRAY(alloc, type, size) \
	(::DuckLib::Internal::Memory::NewArray<type>((alloc), (size)))

#define DL_ALLOC(alloc, size) \
	((alloc).Allocate( size, IAlloc::DEFAULT_ALIGN))

#define DL_ALLOC_A(alloc, size, align) \
	((alloc).Allocate(size, align))

#define DL_REALLOC(alloc, ptr, size) \
	((alloc).Reallocate(ptr, size))
#endif

#define DL_PLACE_NEW(ptr, type, ...) \
	(new (ptr) type(__VA_ARGS__)

#define DL_DELETE(alloc, ptr) \
	(::DuckLib::Internal::Memory::Delete((alloc), (ptr)))

#define DL_DELETE_ARRAY(alloc, ptr) \
	(::DuckLib::Internal::Memory::DeleteArray((alloc), (ptr)))

#define DL_FREE(alloc, ptr) \
	((alloc).Free(ptr))
