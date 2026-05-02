#if defined(_WIN32)
#include <winsock2.h>
#endif
#include <format>
#include <stdexcept>

#include "ducklib/net/net.h"
#include "ducklib/core/logging/logger.h"

namespace ducklib::net
{
void net_initialize()
{
#if defined(_WIN32)
    WSAData data{};

    if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR)
    {
        int error_code = WSAGetLastError();
        throw std::runtime_error(std::format("WinSock startup failed ({})", error_code));
    }
#endif
}

void net_shutdown()
{
#if defined(_WIN32)
    if (WSACleanup() == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        throw std::runtime_error(std::format("WinSock cleanup failed ({})", errorCode));
    }
#endif
}
}
