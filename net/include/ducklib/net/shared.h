#ifndef SHARED_H
#define SHARED_H

#include <string>
#include <cstdint>
#if defined(_WIN32)
#include <winsock.h>
#include <WS2tcpip.h>
#elif defined(__unix__)
#include <arpa/inet.h>
#endif

namespace ducklib::net {
constexpr uint16_t MTU = 1200;

class Address {
public:
    Address() = default;
    Address(std::string_view address, uint16_t port);
    explicit Address(const sockaddr_in &sock_addr);

    [[nodiscard]] uint16_t get_port() const;
    [[nodiscard]] std::string get_address() const;
    [[nodiscard]] sockaddr_in as_sockaddr_in() const; // TODO: Consider moving these to NetClient
    
    bool is_valid() const { return addr_v4_int != 0; }

private:
    uint32_t addr_v4_int;
    uint16_t port;
};

inline auto Address::get_port() const -> uint16_t { return port; }
}
#endif // SHARED_H
