#pragma once

#include <winsock2.h>
#include <cstdio>

namespace ducklib
{
void InitializeNet();
void ShutdownNet();

#define DL_NET_LOG
#define DL_NET_LOG_ERROR(text, ...) (printf(text, __VA_ARGS__))

#define DL_NET_FAIL(text, ...)					\
	do											\
	{											\
		DL_NET_LOG_ERROR(text, __VA_ARGS__);	\
		exit(0);								\
	} while (false)
}
