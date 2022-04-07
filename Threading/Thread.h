#pragma once
#include <cstdint>
#include <Windows.h>

namespace DuckLib
{
class Thread
{
public:

	Thread(uint32_t (*func)(void*), void* data);

	void Join();

private:

#ifdef _WIN32
	HANDLE osHandle;
#else
	void* osHandle;
#endif
};

void YieldThread(uint32_t ms);
}