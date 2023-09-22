#pragma once

#include "TArray.h"

namespace ducklib
{
template <typename T>
class TArrayIt
{
public:
	TArrayIt(T* it);

	T& operator*();
	bool operator!=(const TArrayIt& o);
	void operator++();

private:
	T* it;
};

template <typename T>
TArrayIt<T>::TArrayIt(T* it)
{
	this->it = it;
}

template <typename T>
T& TArrayIt<T>::operator*()
{
	return *it;
}

template <typename T>
bool TArrayIt<T>::operator!=(const TArrayIt& o)
{
	return it != o.it;
}

template <typename T>
void TArrayIt<T>::operator++()
{
	++it;
}

template <typename T>
TArrayIt<T> begin(TArray<T>& array)
{
	return TArrayIt(array.Data());
}

template <typename T>
TArrayIt<T> end(TArray<T>& array)
{
	T* dataPtr = array.Data();
	T* itPtr = dataPtr != nullptr ? &dataPtr[array.Length() - 1] : nullptr;

	return TArrayIt(itPtr);
}
}
