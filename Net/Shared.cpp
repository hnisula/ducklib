#include "Shared.h"

#include <winsock2.h>

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
