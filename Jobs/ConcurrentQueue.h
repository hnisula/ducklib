#pragma once
#include <atomic>
#include "../Core/Memory/IAlloc.h"

namespace DuckLib
{
template <typename T>
class ConcurrentQueue
{
public:

	ConcurrentQueue(uint32_t size);

	bool TryPush(T item);
	bool TryPop(T& item);

private:

	static const uint32_t CACHE_LINE_SIZE = 128;

	struct alignas(CACHE_LINE_SIZE) Slot
	{
		std::atomic<uint64_t> gen; // TODO: There must be a better name...
		T item;
	};

	alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> head;
	alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> tail;

	Slot* slots;
	const uint32_t size;
};

template <typename T>
ConcurrentQueue<T>::ConcurrentQueue(uint32_t size)
	: size(size)
{
	slots = DL_NEW_ARRAY(DefAlloc(), Slot, size);

	for (uint32_t i = 0; i < size; ++i)
		slots[i].gen.store((i << 1) + 1);

	head.store(0);
	tail.store(0);
}

template <typename T>
bool ConcurrentQueue<T>::TryPush(T item)
{
	while (true)
	{
		uint64_t cachedTail = tail.load();
		uint64_t tailItemGen = slots[cachedTail].gen.load();

		if ((cachedTail << 1) + 1 == tailItemGen)
		{
			uint64_t newTail = cachedTail + 1;

			if (tail.compare_exchange_strong(cachedTail, newTail))
			{
				Slot& queueSlot = slots[cachedTail % size];

				queueSlot.item = item;
				queueSlot.gen.store((cachedTail + 1) << 1, std::memory_order_release);

				return true;
			}
		}
		else
		{
			uint64_t oldCachedTail = cachedTail;
			cachedTail = tail.load();

			if (cachedTail == oldCachedTail)
				return false;
		}
	}
}

template <typename T>
bool ConcurrentQueue<T>::TryPop(T& item)
{
	while (true)
	{
		uint64_t cachedHead = head.load();
		uint64_t headItemGen = slots[cachedHead].gen;

		if ((cachedHead + 1) << 1 == headItemGen)
		{
			uint64_t newHead = cachedHead + 1;

			if (head.compare_exchange_strong(cachedHead, newHead))
			{
				Slot& queueSlot = slots[cachedHead % size];
				
				item = queueSlot.item;
				queueSlot.gen.store(((cachedHead + 1) << 1) + 1, std::memory_order_release);

				return true;
			}
		}
		else
		{
			uint64_t oldCachedHead = cachedHead;
			cachedHead = head.load();

			if (cachedHead == oldCachedHead)
				return false;
		}
	}
}
}