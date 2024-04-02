#ifndef SOCKET_H
#define SOCKET_H

#include <span>
#include <winsock2.h>
#include "Shared.h"

namespace ducklib::net
{
class Socket
{
public:
    Socket() = delete;
    Socket(const Socket& other) = delete;
    Socket(Socket&& other) noexcept = delete;
    /**
     *\param bindPort Port to bind socket to. 0 = port assigned by OS.
     */
    explicit Socket(uint16_t bindPort = 0);
    ~Socket();

    [[nodiscard]] auto get_port() const -> int;

    [[nodiscard]] auto send(Address to, std::span<const std::byte> data) const -> int;
    auto receive(Address* from, std::span<std::byte> receive_buffer) const -> int;

    // TODO: Consider adding a Close() function

    auto operator=(const Socket& other) -> Socket& = delete;
    auto operator=(Socket&& other) noexcept -> Socket& = delete;

private:
    SOCKET socket_handle;
    Address address;
};
}

#endif // SOCKET_H
