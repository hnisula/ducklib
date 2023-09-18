#pragma once
#include <cstdint>

#include "Shared.h"
#include "Socket.h"

namespace ducklib
{
class NetClient
{
public:
	NetClient(uint16_t bindPort = 0);

	void SendPacket(const Address* to, const uint8_t* data, int dataSize);

private:
	static constexpr int MAX_PACKET_SIZE = 1024;
	static constexpr int HEADER_SIZE = 4;
	static constexpr int MAX_PACKET_PAYLOAD_SIZE = MAX_PACKET_SIZE - HEADER_SIZE;
	static constexpr int MAX_FRAGMENTS = 256;

	enum class PacketType
	{
		REGULAR = 0,
		FRAGMENT = 1
	};

	static void WritePacketHeader(uint8_t* packetBuffer, PacketType packetType, uint8_t specialByte, uint16_t sequence);

	uint8_t packetSendBuffer[MAX_PACKET_SIZE];

	uint16_t nextSequence = 0;

	Socket socket;
};
}
