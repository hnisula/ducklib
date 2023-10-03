#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <array>

#include "Shared.h"

namespace ducklib
{
Address::Address()
	: ip(0)
	, port(0) {}

Address::Address(const std::string& address, uint16_t port)
	: port(port)
{
	IN_ADDR in_addr;
	const int result = inet_pton(AF_INET, address.data(), &in_addr);

	if (result != 1)
		throw std::exception("Failed to parse address");

	ip = in_addr.S_un.S_addr;
}

Address::Address(uint32_t ip, uint16_t port)
	: ip(ip)
	, port(port)
{}

std::string Address::to_string() const
{
	auto ip_buffer = std::array<char, INET_ADDRSTRLEN>{};
	in_addr addr_struct;

	addr_struct.S_un.S_addr = ip;

	if (!inet_ntop(AF_INET, &addr_struct, ip_buffer.data(), INET_ADDRSTRLEN))
		throw std::exception("Failed to convert network address to string");

	auto result = std::string{ INET_ADDRSTRLEN, '\0' };

	return std::string{ ip_buffer.data() };
}

std::pair<std::string, uint16_t> split_address(const std::string& addr)
{
	auto split_index = addr.find(':');

	if (split_index == std::string::npos)
		throw std::exception("Invalid address");

	return { addr.substr(0, split_index), stoi(addr.substr(split_index + 1)) };
}
}
