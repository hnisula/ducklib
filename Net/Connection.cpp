#include "Connection.h"

#include <cassert>

namespace ducklib
{
Connection Connection::Connect(Address address, Socket* socket)
{
	assert(socket);

	Connection connection(address, socket);
	ConnectionRequest connectionRequestPacket;

	socket->Send(address, )

	connection.state = ConnectionState::CONNECTION_REQUESTED;

	return connection;
}
}
