#pragma once
#include <cstdint>
#include "../IAlloc.h"

namespace DuckLib
{
template <typename T>
class TArray
{
public:
	TArray();
	TArray(uint64_t initialCapacity);
	TArray(void* externalArray, uint64_t size, uint64_t capacity = size, IAlloc* alloc = nullptr);
	~TArray();

	void Append(const T&& item);

	bool Contains(const T& item) const;

	void Resize(uint64_t newCapacity);

protected:
	bool EnsureCapacity(uint64_t requiredCapacity);

	IAlloc* alloc;
	T* array;
	uint32_t size;
	uint32_t capacity;
	bool isExternalArray;
};

template <typename T>
TArray<T>::TArray()
	: alloc(DefAlloc())
	, array(nullptr)
	, size(0)
	, capacity(0)
	, isExternalArray(false) {}

template <typename T>
TArray<T>::TArray(uint64_t initialCapacity)
	: TArray()
{
	EnsureCapacity(initialCapacity);
}

template <typename T>
TArray<T>::TArray(void* externalArray, uint64_t size, uint64_t capacity, IAlloc* alloc)
{
	this->alloc = alloc;
	array = (uint32_t*)externalArray;
	this->size = size;
	this->capacity = capacity;
	isExternalArray = true;
}

template <typename T>
TArray<T>::~TArray()
{
	if (isExternalArray)
		return;

	if (array)
		alloc->Free(array);
}

template <typename T>
void TArray<T>::Append(const T&& item)
{
	if (!EnsureCapacity(size + 1))
		throw std::runtime_error("Failed to append in array because of limited capacity");

	array[sizeof(T) * size++] = std::move(item);
}

template <typename T>
bool TArray<T>::Contains(const T& item) const
{
	for (uint32_t i = 0; i < size; ++i)
		if (array[i] == item)
			return true;

	return false;
}

template <typename T>
void TArray<T>::Resize(uint64_t newCapacity)
{
	if (array)
		alloc->Reallocate(array, newCapacity);
	else
		alloc->Allocate(newCapacity, alignof(T));

	capacity = newCapacity;
}

template <typename T>
bool TArray<T>::EnsureCapacity(uint64_t requiredCapacity)
{
	if (requiredCapacity <= capacity)
		return true;

	if (!alloc)
		return false;

	uint64_t exponentiallyIncreasedSize = capacity * 1.5f;

	Resize(requiredCapacity <= exponentiallyIncreasedSize ? exponentiallyIncreasedSize : requiredCapacity);

	return true;
}
}
