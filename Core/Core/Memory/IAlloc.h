#pragma once
#include <source_location>
#include "AllocTracker.h"
#include "MemoryInternal.h"

namespace DuckLib
{
class IAlloc;

IAlloc& DefAlloc();

class IAlloc
{
public:
	virtual ~IAlloc() { }

#ifdef DL_TRACK_ALLOCS
	void* Allocate(
		uint64_t size,
		uint8_t align,
		const std::source_location& sourceLocation);
	void* Reallocate(
		void* ptr,
		uint64_t size,
		const std::source_location& sourceLocation);
#else
	void* Allocate(uint64_t size, uint8_t align = DEFAULT_ALIGN);
	void* Reallocate(void* ptr, uint64_t size);
#endif

	void Free(void* ptr);

	static constexpr uint8_t DEFAULT_ALIGN = 4;

protected:

	virtual void* AllocateInternal(uint64_t size, uint8_t align) = 0;
	virtual void* ReallocateInternal(void* ptr, uint64_t size) = 0;

	virtual void FreeInternal(void* ptr) = 0;
};

template <typename T, typename... Args>
T* NewA(
	IAlloc& alloc = DefAlloc(),
	Args&&... args,
	const std::source_location& sourceLocation = std::source_location::current())
{
	T* ptr = new(alloc.Allocate(sizeof(T), alignof(T))) T{std::forward<Args>(args)...};

#ifdef DL_TRACK_ALLOCS
	Internal::Memory::GetAllocTracker().Track(
		ptr,
		sizeof(T),
		sourceLocation.file_name(),
		sourceLocation.function_name(),
		sourceLocation.line());
#endif

	return ptr;
}

template <typename T, typename... Args>
T* New(Args... args, const std::source_location& sourceLocation = std::source_location::current())
{
	return NewA<T>(DefAlloc(), std::forward<Args>(args)..., sourceLocation);
}

namespace Internal::Memory
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
	((alloc).Free(ptr)
}
