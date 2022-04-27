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

	void Append(T&& item);
	void Append(const T& item);

	bool Contains(const T& item) const;

	uint32_t Size() const;
	uint32_t Capacity() const;
	void Resize(uint32_t newCapacity);

	T& operator [](uint32_t i);
	const T& operator [](uint32_t i) const;

protected:
	bool EnsureCapacity(uint32_t requiredCapacity);

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
void TArray<T>::Append(T&& item)
{
	if (!EnsureCapacity(size + 1))
		throw std::runtime_error("Failed to append in array because of limited capacity");

	array[size++] = std::move(item);
}

template <typename T>
void TArray<T>::Append(const T& item)
{
	if (!EnsureCapacity(size + 1))
		throw std::runtime_error("Failed to append in array because of limited capacity");

	array[size++] = item;
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
uint32_t TArray<T>::Size() const
{
	return size;
}

template <typename T>
uint32_t TArray<T>::Capacity() const
{
	return capacity;
}

template <typename T>
void TArray<T>::Resize(uint32_t newCapacity)
{
	if (array)
		array = (T*)alloc->Reallocate(array, newCapacity * sizeof(T));
	else
		array = (T*)alloc->Allocate(newCapacity * sizeof(T), alignof(T));

	capacity = newCapacity;
}

template <typename T>
T& TArray<T>::operator[](uint32_t i)
{
	return array[i];
}

template <typename T>
const T& TArray<T>::operator[](uint32_t i) const
{
	return array[i];
}

template <typename T>
bool TArray<T>::EnsureCapacity(uint32_t requiredCapacity)
{
	if (requiredCapacity <= capacity)
		return true;

	if (!alloc)
		return false;

	uint32_t exponentiallyIncreasedSize = (uint32_t)((float)(capacity == 0 ? 4 : capacity) * 1.5f);

	Resize(requiredCapacity <= exponentiallyIncreasedSize ? exponentiallyIncreasedSize : requiredCapacity);

	return true;
}
}
