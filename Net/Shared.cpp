#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>
#include "Shared.h"

namespace ducklib
{
Address::Address(const Address& address, uint16 port)
	: addrV4(address.addrV4)
	, port(port)
{}

Address::Address(const sockaddr_in& sockAddr)
{
	addrV4 = sockAddr.sin_addr.s_addr;
	port = ntohs(sockAddr.sin_port);
}

Address::Address(const char* address)
{
	// First split address into IP and port
	char addressCopy[64];
	strcpy_s(addressCopy, 64, address);
	char* portDelimiter = strchr(addressCopy, ':');
	port = 0;	// TODO: Come up with some default?

	if (portDelimiter)
	{
		port = (uint16)std::stoi(portDelimiter + 1);
		*portDelimiter = '\0';
	}

	IN_ADDR inAddr;
	int result = inet_pton(AF_INET, addressCopy, &inAddr);

	if (result != 1)
		throw std::exception("Failed to parse address");

	addrV4 = inAddr.S_un.S_addr;
}

sockaddr_in Address::AsSockAddrIn() const
{
	sockaddr_in sockAddr{};

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = addrV4;

	return sockAddr;
}
}
