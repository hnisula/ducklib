#ifndef SOCKET_H
#define SOCKET_H

#include <span>

#if defined(_WIN32)
#include <winsock2.h>
#include <WS2tcpip.h>
using SocketHandle = SOCKET;
constexpr SocketHandle INVALID_SOCK = INVALID_SOCKET;
#elif defined(__unix__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
using SocketHandle = int;
constexpr SocketHandle INVALID_SOCKET_VALUE = -1;
#endif

#include "shared.h"

namespace ducklib::net
{
class Socket
{
public:
    Socket() = delete;
    Socket(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    /**
     *\param bindPort Port to bind socket to. 0 = port assigned by OS.
     */
    explicit Socket(uint16_t bindPort = 0);
    ~Socket();

    [[nodiscard]]
    auto get_port() const -> uint16_t;

    [[nodiscard]]
    auto send(Address to, std::span<const std::byte> data) const -> size_t;
    auto receive(Address& from, std::span<std::byte> receive_buffer) const -> size_t;

    // TODO: Consider adding a Close() function

    auto operator=(const Socket& other) -> Socket& = delete;
    auto operator=(Socket&& other) noexcept -> Socket& = delete;

private:
    SocketHandle socket_handle;
    Address address;
};
}

#endif // SOCKET_H
