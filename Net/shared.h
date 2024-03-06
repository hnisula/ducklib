#pragma once

#include <WinSock2.h>
#include <string>

namespace ducklib
{
class Address
{
public:
	/// Constructs an address to 0.0.0.0 and is invalid
	Address();
	/// @param port If 0 it will be chosen by OS
	explicit Address(const std::string& address, uint16_t port);
	/// @param port If 0 it will be chosen by OS
	explicit Address(uint32_t ip, uint16_t port);
	Address(const Address& other) = default;
	Address(Address&& other) = default;
	~Address() = default;

	[[nodiscard]] uint16_t get_port() const;
	[[nodiscard]] uint32_t get_ip() const;
	[[nodiscard]] std::string to_string() const;
	[[nodiscard]] bool is_valid() const;

	Address& operator = (const Address& other) = default;
	Address& operator = (Address&& other) = default;

protected:
	/// Stored as net (big) endian
	uint32_t ip;
	/// Stored as net (big) endian
	uint16_t port;
};

inline uint16_t Address::get_port() const { return port; }
inline uint32_t Address::get_ip() const { return ip; }
inline bool Address::is_valid() const { return ip != 0; }

std::pair<std::string, uint16_t> split_address(const std::string& addr);
}
