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
	void Sleep(uint32_t ms);

private:

#ifdef _WIN32
	HANDLE osHandle;
#endif
};
}
