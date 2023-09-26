#pragma once
#include "Shared.h"
#include "Socket.h"

namespace ducklib
{
enum class ConnectionState
{
	DISCONNECTED,
	CONNECTION_REQUESTED,
	CONNECTED
};

enum class ConnectionPacketType
{
	REQUEST,
	ACCEPT,
	DISCONNECT
};

struct ConnectionRequest
{
	ConnectionPacketType type {};
};

struct ConnectionAccept
{
	ConnectionPacketType type;
};

class Connection
{
public:
	static Connection Connect(Address address, Socket* socket);
	static Connection Accept(/*ConnectionPacket connectionPacket*/, Socket* socket);
protected:
	Connection(Address address, Socket* socket);

	Socket* socket;
	Address address;
	ConnectionState state;
};
}
