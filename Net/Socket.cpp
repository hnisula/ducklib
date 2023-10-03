#include "Socket.h"
#include "Net.h"

#include <cassert>
#include <exception>
#include <WS2tcpip.h>

namespace ducklib
{
Socket::Socket(uint16_t bindPort)
	: socketHandle(INVALID_SOCKET)
{
	// Create socket and set options
	socketHandle = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketHandle == INVALID_SOCKET)
		DL_NET_FAIL("Failed to create socket");

	// Bind socket
	sockaddr_in socketAddress;

	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons(bindPort);
	socketAddress.sin_family = AF_INET;

	if (bind(socketHandle, (sockaddr*)&socketAddress, sizeof(socketAddress)) < 0)
		DL_NET_FAIL("Failed to bind socket");

	// Get which port was bound
	sockaddr_in boundSocketAddress;
	int boundSocketAddressSize = sizeof boundSocketAddress;
	int boundNameResult = getsockname(socketHandle, (sockaddr*)&boundSocketAddress, &boundSocketAddressSize);

	if (boundNameResult != 0)
		DL_NET_FAIL("Failed to get bound address of socket (%d)", WSAGetLastError());

	this->address = sockaddr_to_address(boundSocketAddress);

	// Set non-blocking mode
	DWORD nonBlockFlag = 1;
	if (ioctlsocket(socketHandle, FIONBIO, &nonBlockFlag) != 0)
		DL_NET_FAIL("Failed to set non-blocking mode on socket");
}

Socket::~Socket()
{
	assert(socketHandle);

	closesocket(socketHandle);
}

int Socket::get_port() const
{
	assert(socketHandle);

	return address.get_port();
}

Address Socket::sockaddr_to_address(const sockaddr_in& sock_addr)
{
	return Address{ (uint32_t)sock_addr.sin_addr.S_un.S_addr, sock_addr.sin_port };
}

sockaddr_in Socket::address_to_sockaddr(const Address& address)
{
	sockaddr_in sock_addr{};

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = address.get_port();
	sock_addr.sin_addr.S_un.S_addr = address.get_ip();

	return sock_addr;
}

int Socket::send(const Address& dest, const uint8_t* data, uint32_t dataSize)
{
	if (socketHandle == INVALID_SOCKET)
		DL_NET_FAIL("Trying to send data over uninitialized socket");

	sockaddr_in socketAddress = address_to_sockaddr(dest);
	HRESULT result = sendto(socketHandle, (const char*)data, (int)dataSize, 0, (sockaddr*)&socketAddress, sizeof(socketAddress));

	if (result == SOCKET_ERROR)
		DL_NET_FAIL("Failed to send data over socket");

	return result;
}

int Socket::receive(Address& fromAddress, uint8_t* buffer, uint32_t bufferSize)
{
	assert(socketHandle != INVALID_SOCKET);
	assert(buffer);
	assert(bufferSize > 0);

	sockaddr_in socketAddress;
	int socketAddressSize = sizeof(socketAddress);
	HRESULT result = recvfrom(socketHandle, (char*)buffer, (int)bufferSize, 0, (sockaddr*)&socketAddress, &socketAddressSize);

	// TODO: Check socket address size value?

	// TODO: Propagate this out to the caller
	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();

		if (errorCode == WSAEWOULDBLOCK)
			return 0;

		DL_NET_FAIL("Failed to receive data over socket");
	}

	fromAddress = sockaddr_to_address(socketAddress);

	return result;
}
}
