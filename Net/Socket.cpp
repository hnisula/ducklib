#include "Socket.h"

#include <cassert>
#include <exception>

namespace DuckLib
{
Socket::Socket(const Address& address, uint32 bufferSizes)
	: socketHandle(INVALID_SOCKET)
{
	// Create socket and set options
	socketHandle = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketHandle == INVALID_SOCKET)
		throw std::exception("Failed to create socket");

	if (setsockopt(socketHandle, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSizes, sizeof(uint32)) != 0)
		throw std::exception("Failed to set receive buffer size on socket");

	if (setsockopt(socketHandle, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSizes, sizeof(uint32)) != 0)
		throw std::exception("Failed to set send buffer size on socket");

	// Bind socket
	sockaddr_in socketAddress = address.AsSockAddrIn();

	if (bind(socketHandle, (sockaddr*)&socketAddress, sizeof socketAddress) < 0)
		throw std::exception("Failed to bind socket");

	// Set non-blocking mode
	DWORD nonBlockFlag = 1;
	if (ioctlsocket(socketHandle, FIONBIO, &nonBlockFlag) != 0)
		throw std::exception("Failed to set non-blocking mode on socket");
}
Socket::~Socket()
{
	if (socketHandle == INVALID_SOCKET)
		return;

	closesocket(socketHandle);
}
bool Socket::IsOpen() const
{
	return socketHandle != INVALID_SOCKET;
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
	HRESULT result = recvfrom(socketHandle, (char*)buffer, (int)bufferSize, 0, (sockaddr*)&socketAddress, nullptr);
}
}
