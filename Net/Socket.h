#pragma once

#include "Shared.h"

namespace ducklib
{
class Socket
{
public:
	/**
	 *\param bindPort Port to bind socket to. 0 = OS chooses.
	 */
	explicit Socket(uint16_t bindPort = 0);
	~Socket();

	int get_port() const;

	int send(const Address& dest, const uint8_t* data, uint32_t dataSize);
	int receive(Address& fromAddress, uint8_t* buffer, uint32_t bufferSize);

	// TODO: Consider adding a Close() function

private:
	static Address sockaddr_to_address(const sockaddr_in& sock_addr);
	static sockaddr_in address_to_sockaddr(const Address& address);
	
	SOCKET socketHandle;
	Address address;
};
}
