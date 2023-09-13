#pragma once
#include <ws2def.h>

#include "../Core/Types.h"

namespace ducklib
{
class Address
{
public:
	Address() = default;
	Address(const Address& address, uint16 port);
	Address(const sockaddr_in& sockAddr);
	/**
	 * @param address IP address and can contain port.
	 */
	Address(const char* address);

	uint16 GetPort() const;

	sockaddr_in AsSockAddrIn() const;

protected:
	uint32 addrV4;
	uint16 port;
};

inline uint16 Address::GetPort() const { return port; }
}
