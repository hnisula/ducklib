#include "ducklib/net/socket.h"
#include "ducklib/net/net.h"

#include <cassert>
#include <exception>

namespace ducklib::net {
Socket::Socket(Socket&& other) noexcept
    : socket_handle(INVALID_SOCKET_VALUE)
    , address("127.0.0.1", 0) {
    auto temp_socket = other.socket_handle;
    auto temp_address = other.address;
    other.socket_handle = socket_handle;
    other.address = address;
    socket_handle = temp_socket;
    address = temp_address;
}

Socket::Socket(uint16_t bindPort)
    : socket_handle(INVALID_SOCKET_VALUE)
    , address() {
    // Create socket and set options
    socket_handle = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_handle == INVALID_SOCKET_VALUE)
        net_log_error("Failed to create socket");

    // Bind socket
    sockaddr_in socketAddress{};

    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(bindPort);
    socketAddress.sin_family = AF_INET;

    if (bind(socket_handle, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(socketAddress)) < 0)
        net_log_error("Failed to bind socket");

    // Get which port was bound
    sockaddr_in boundSocketAddress{};
#if defined(_WIN32)
    auto boundSocketAddressSize = sizeof(boundSocketAddress);
#elif defined(__unix__)
    socklen_t boundSocketAddressSize = sizeof(boundSocketAddress);
#endif
    auto boundNameResult = getsockname(
        socket_handle,
        reinterpret_cast<sockaddr*>(&boundSocketAddress),
        &boundSocketAddressSize);

    if (boundNameResult != 0) {
#if defined(_WIN32)
        auto error = WSAGetLastError();
#elif defined(__unix__)
        auto error = errno;
#endif
        net_log_error("Failed to get bound address of socket (%d)", error);
    }

    this->address = Address(boundSocketAddress);

    // Set non-blocking mode
#if defined(_WIN32)
    DWORD nonBlockFlag = 1;
    if (ioctlsocket(socket_handle, FIONBIO, &nonBlockFlag) != 0)
        net_log_error("Failed to set non-blocking mode on socket");
#elif defined(__unix__)
    int flags = fcntl(socket_handle, F_GETFL, 0);
    fcntl(socket_handle, F_SETFL, flags | O_NONBLOCK);
#endif
}

Socket::~Socket() {
    if (socket_handle != INVALID_SOCKET_VALUE) {
#if defined(_WIN32)
        closesocket(socket_handle);
#elif defined(__unix__)
        close(socket_handle);
#endif
    }
    
    socket_handle = INVALID_SOCKET_VALUE;
}

auto Socket::get_port() const -> uint16_t {
    assert(socket_handle != INVALID_SOCKET_VALUE);

    return address.get_port();
}

auto Socket::send(Address to, std::span<const std::byte> data) const -> size_t {
    assert(socket_handle != INVALID_SOCKET_VALUE);

    auto socketAddress = to.as_sockaddr_in();
    auto sent_bytes = sendto(
        socket_handle,
        reinterpret_cast<const char*>(data.data()),
        static_cast<int>(data.size()),
        0,
        reinterpret_cast<sockaddr*>(&socketAddress),
        sizeof(socketAddress));

    if (sent_bytes == -1) {
        net_log_error("Failed to send data over socket");
    }

    return sent_bytes;
}

auto Socket::receive(Address& from, std::span<std::byte> receive_buffer) const -> size_t {
    assert(socket_handle != INVALID_SOCKET_VALUE);
    assert(&from);

    sockaddr_in socketAddress{};
#if defined(_WIN32)
    auto socketAddressSize = sizeof(socketAddress);
#elif defined(__unix__)
    socklen_t socketAddressSize = sizeof(socketAddress);
#endif
    auto received_bytes = recvfrom(
        socket_handle,
        reinterpret_cast<char*>(receive_buffer.data()),
        static_cast<int>(receive_buffer.size()),
        0,
        reinterpret_cast<sockaddr*>(&socketAddress),
        &socketAddressSize);

    // TODO: Check socket address size value?

    // TODO: Propagate this out to the caller
    if (received_bytes == -1) {
#if defined(_WIN32)
        if (WSAGetLastError() == WSAEWOULDBLOCK)
#elif defined(__unix__)
        if (errno == EWOULDBLOCK || errno == EAGAIN)
#endif
        {
            return 0;
        }

        net_log_error("Failed to receive data over socket");
    }

    new(&from) Address(socketAddress);

    return received_bytes;
}
}
