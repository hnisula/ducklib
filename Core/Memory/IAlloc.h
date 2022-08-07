#pragma once
#include "AllocTracker.h"

namespace DuckLib
{
class IAlloc;

IAlloc* DefAlloc();

class IAlloc
{
public:
	IAlloc();
	virtual ~IAlloc() { }

	void* Allocate(uint64 size, uint8_t align = DEFAULT_ALIGN);
	void* Reallocate(void* ptr, uint64 size);
	void Free(void* ptr);

	template <typename T>
	T* Allocate(uint32 count = 1);

	template <typename T, typename... TArgs>
	T* New(TArgs&&... args);
	template <typename T>
	void Delete(T* ptr);

	// TODO: Come up with a good way to support this. Do all implementations then require a header to keep track of count or total size?
	// template <typename T>
	// T* NewArray(uint64 count);
	// template <typename T>
	// void DeleteArray(T* ptr);

	static constexpr uint8_t DEFAULT_ALIGN = 4;

protected:
	virtual void* AllocateInternal(uint64 size, uint8_t align = DEFAULT_ALIGN) = 0;
	virtual void* ReallocateInternal(void* ptr, uint64 newSize) = 0;
	virtual void FreeInternal(void* ptr) = 0;

	// totalSize includes possible headers, alignment and similar extra data
	uint64 totalAllocatedSize;
	uint64 allocatedSize;
	uint32 allocationCount;
};

template <typename T>
T* IAlloc::Allocate(uint32 count)
{
	return (T*)Allocate(sizeof(T) * count, alignof(T));
}

template <typename T, typename ... TArgs>
T* IAlloc::New(TArgs&&... args)
{
	return new (Allocate(sizeof(T), alignof(T))) T(std::forward<TArgs>(args)...);
}

template <typename T>
void IAlloc::Delete(T* ptr)
{
	ptr->~T();
	Free(ptr);
}

// template <typename T>
// T* IAlloc::NewArray(uint64 count)
// {
// 	T* array = Allocate(sizeof(T) * count, alignof(T));
//
// 	for (uint64 i = 0; i < count; ++i)
// 		new (&array[i]) T();
//
// 	return array;
// }
//
// template <typename T>
// void IAlloc::DeleteArray(T* ptr)
// {
// 	for
// }

}
