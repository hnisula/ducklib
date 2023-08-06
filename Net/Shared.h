#pragma once
#include "../Core/Types.h"

namespace DuckLib
{
class Address
{
public:
	uint32 GetAddressUint() const;
	uint16 GetPort() const;

protected:
	uint32 addrV4;
	uint16 port;
};

inline uint32 Address::GetAddressUint() const { return addrV4; }
inline uint16 Address::GetPort() const { return port; }
}
