#include "Thread.h"

namespace DuckLib
{
Thread::Thread(uint32_t (* func)(void*), void* data)
{
#ifdef _WIN32
	uint32_t threadId {};
	
	osHandle = CreateThread(nullptr,
		0,
		(LPTHREAD_START_ROUTINE)func,
		data,
		0,
		(LPDWORD)&threadId);
#endif
}

void Thread::Join()
{
	WaitForSingleObject(osHandle, INFINITE);
}

void Thread::Sleep(uint32_t ms)
{
	::Sleep(ms);
}
}