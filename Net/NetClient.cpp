#include <winsock2.h>
#include <cassert>
#include "NetClient.h"
#include "Net.h"

namespace ducklib
{
NetClient::NetClient(uint16_t bindPort)
	: socket(bindPort)
{
	memset(packetSendBuffer, 0, sizeof packetSendBuffer);
}

void NetClient::SendPacket(const Address* to, const uint8_t* data, int dataSize)
{
	assert(to);
	assert(data);
	assert(dataSize > 0);

	if (dataSize > MAX_PACKET_SIZE)
	{
		// Ceiling function from Number Conversion (2001) by Roland Backhouse
		const int fragmentCount = (dataSize + MAX_PACKET_PAYLOAD_SIZE - 1) / MAX_PACKET_PAYLOAD_SIZE;

		for (int i = 0; i < fragmentCount; ++i)
		{
			// Construct fragment packet
			const int fragmentSize = dataSize - i * MAX_PACKET_PAYLOAD_SIZE;
			const int packetSize = HEADER_SIZE + fragmentSize;
			WritePacketHeader(packetSendBuffer, PacketType::FRAGMENT, (uint8_t)i, nextSequence);
			memcpy(&packetSendBuffer[HEADER_SIZE], &data[(uint64_t)i * MAX_PACKET_PAYLOAD_SIZE], fragmentSize);

			// Send packet
			const int sizeSent = socket.Send(to, packetSendBuffer, packetSize);

			if (sizeSent != dataSize)
				DL_NET_LOG_ERROR("Failed to send fragment (#%d) packet of size %d", i, packetSize);
		}
	}
	else
	{
		// Construct regular packet
		const int packetSize = HEADER_SIZE + dataSize;
		WritePacketHeader(packetSendBuffer, PacketType::REGULAR, 0, nextSequence);
		memcpy(&packetSendBuffer[HEADER_SIZE], data, dataSize);

		// Send packet
		const int sizeSent = socket.Send(to, packetSendBuffer, packetSize);

		if (sizeSent != dataSize)
			DL_NET_LOG_ERROR("Failed to send packet of size %d", packetSize);
	}

	// TODO: Store sequence and timestamp of when this packet was sent to track RTT

	++nextSequence;
}

void NetClient::WritePacketHeader(uint8_t* packetBuffer, PacketType packetType, uint8_t specialByte, uint16_t sequence)
{
	assert(packetBuffer);

	// Packet type
	packetBuffer[0] = (uint8_t)packetType;

	// Special byte for some packet types' extra stuff (or just 0-padding)
	packetBuffer[1] = specialByte;

	// Packet sequence number
	const uint16_t netSequenceNumber = htons(sequence);
	packetBuffer[2] = (netSequenceNumber >> 8) & 0xFF;
	packetBuffer[3] = netSequenceNumber & 0xFF;
}
}
