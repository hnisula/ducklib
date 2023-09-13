#pragma once

#include <winsock2.h>

#include "Shared.h"

namespace ducklib
{
class Socket
{
public:
	Socket(const Address* address = nullptr, uint32 bufferSizes = DEFAULT_BUFFER_SIZES);
	~Socket();

	int GetPort() const;

	int32 Send(const Address& address, const void* data, uint32 dataSize);
	int32 Receive(Address* fromAddress, void* buffer, uint32 bufferSize);

protected:
	static constexpr uint32 DEFAULT_BUFFER_SIZES = 256 * 1024;
	
	SOCKET socketHandle;
	Address address;
};
}
