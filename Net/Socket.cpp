#include "Socket.h"

#include <cassert>
#include <exception>
#include <WS2tcpip.h>

namespace DuckLib
{
Socket::Socket(const Address* address, uint32 bufferSizes)
	: socketHandle(INVALID_SOCKET)
{
	// Create socket and set options
	socketHandle = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketHandle == INVALID_SOCKET)
		throw std::exception("Failed to create socket");

	if (setsockopt(socketHandle, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSizes, sizeof(uint32)) != 0)
		throw std::exception("Failed to set receive buffer size on socket");

	if (setsockopt(socketHandle, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSizes, sizeof(uint32)) != 0)
		throw std::exception("Failed to set send buffer size on socket");

	// Bind socket
	sockaddr_in socketAddress;

	if (address)
	{
		socketAddress = address->AsSockAddrIn();
	}
	else
	{
		addrinfo hints;
		addrinfo* addressResult;

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE;

		if (getaddrinfo(nullptr, "0", &hints, &addressResult) != 0)
			throw std::exception("Failed to get local address info");

		const auto socketAddressResult = (sockaddr_in*)addressResult->ai_addr;
		socketAddress.sin_family = AF_INET;
		socketAddress.sin_port = socketAddressResult->sin_port;
		socketAddress.sin_addr = socketAddressResult->sin_addr;
	}

	if (bind(socketHandle, (sockaddr*)&socketAddress, sizeof socketAddress) < 0)
		throw std::exception("Failed to bind socket");

	// Get which port was bound
	sockaddr_in boundSocketAddress;
	int boundSocketAddressSize = sizeof boundSocketAddress;
	int boundNameResult = getsockname(socketHandle, (sockaddr*)&boundSocketAddress, &boundSocketAddressSize);

	if (boundNameResult != 0)
	{
		int error = WSAGetLastError();
		throw std::exception("Failed to get bound address of socket");
	}

	this->address = Address(boundSocketAddress);

	// Set non-blocking mode
	DWORD nonBlockFlag = 1;
	if (ioctlsocket(socketHandle, FIONBIO, &nonBlockFlag) != 0)
		throw std::exception("Failed to set non-blocking mode on socket");
}
Socket::~Socket()
{
	if (socketHandle == INVALID_SOCKET)
		return;

	int result = closesocket(socketHandle);
	int errorCode = WSAGetLastError();
}

int Socket::GetPort() const
{
	return address.GetPort();
}

int32 Socket::Send(const Address& address, const void* data, uint32 dataSize)
{
	if (socketHandle == INVALID_SOCKET)
		throw std::exception("Trying to send data over uninitialized socket");

	sockaddr_in socketAddress = address.AsSockAddrIn();
	HRESULT result = sendto(socketHandle, (const char*)data, (int)dataSize, 0, (sockaddr*)&socketAddress, sizeof socketAddress);

	if (result == SOCKET_ERROR)
		throw std::exception("Failed to send data over socket");

	return result;
}

int32 Socket::Receive(Address* fromAddress, void* buffer, uint32 bufferSize)
{
	if (socketHandle == INVALID_SOCKET)
		throw std::exception("Trying to receive data from uninitialized socket");

	sockaddr_in socketAddress;
	int socketAddressSize = sizeof socketAddress;
	HRESULT result = recvfrom(socketHandle, (char*)buffer, (int)bufferSize, 0, (sockaddr*)&socketAddress, &socketAddressSize);

	// TODO: Check socket address size value?

	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();

		if (errorCode == WSAEWOULDBLOCK)
			return 0;

		throw std::exception("Failed to receive data over socket");
	}

	new (fromAddress) Address(socketAddress);

	return result;
}
}
