#pragma once

#include <winsock2.h>

#include "Shared.h"

namespace ducklib
{
class Socket
{
public:
	/**
	 *\param bindPort Port to bind socket to. 0 = OS chooses.
	 */
	Socket(uint16_t bindPort = 0, uint32 bufferSizes = DEFAULT_BUFFER_SIZES);
	~Socket();

	int GetPort() const;

	int Send(const Address* dest, const uint8_t* data, uint32 dataSize);
	int Receive(Address* fromAddress, uint8_t* buffer, uint32 bufferSize);

protected:
	static constexpr uint32 DEFAULT_BUFFER_SIZES = 256 * 1024;
	
	SOCKET socketHandle;
	Address address;
};
}
