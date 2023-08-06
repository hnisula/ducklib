#pragma once

#include <winsock2.h>

#include "Shared.h"

namespace DuckLib
{
class Socket
{
public:
	Socket(const Address& address, uint32 bufferSizes = DEFAULT_BUFFER_SIZES);

protected:
	static constexpr uint32 DEFAULT_BUFFER_SIZES = 256 * 1024;
	
	SOCKET socket;
};
}
