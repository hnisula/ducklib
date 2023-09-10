#include "Shared.h"

#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>

namespace DuckLib
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

Address::Address(const char8* address)
{
	// First split address into IP and port
	char* portDelimiter = strchr(address, ':');
	port = 0;	// TODO: Come up with some default?

	if (portDelimiter)
		port = (uint16)std::stoi(portDelimiter + 1);

	addrV4 = inet_pton(address);
}

sockaddr_in Address::AsSockAddrIn() const
{
	sockaddr_in sockAddr{
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr.s_addr = addrV4
	};

	return sockAddr;
}
}
