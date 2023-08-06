#include "Socket.h"

#include <exception>

namespace DuckLib
{
Socket::Socket(const Address& address, uint32 bufferSizes)
{
	// Create socket and set options
	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket == INVALID_SOCKET)
		throw std::exception("Failed to create socket");

	if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSizes, sizeof(uint32)) != 0)
		throw std::exception("Failed to set receive buffer size on socket");

	if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSizes, sizeof(uint32)) != 0)
		throw std::exception("Failed to set send buffer size on socket");

	// Bind socket
	sockaddr_in socketAddress{};

	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(address.GetPort());
	socketAddress.sin_addr.s_addr = address.GetAddressUint();

	if (bind(socket, (sockaddr*)&socketAddress, sizeof socketAddress) < 0)
		throw std::exception("Failed to bind socket");

	// Set non-blocking mode
	DWORD nonBlockFlag = 1;
	if (ioctlsocket(socket, FIONBIO, &nonBlockFlag) != 0)
		throw std::exception("Failed to set non-blocking mode on socket");
}
}
