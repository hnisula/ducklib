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

	bool SendPacket(const Address* to, const uint8_t* data, int dataSize);
	void ReceivePacket();

private:
	static constexpr int BASE_HEADER_SIZE = 3;
	static constexpr int FRAGMENT_SUB_HEADER_SIZE = 2;
	static constexpr int MAX_PACKET_SIZE = 1024;
	static constexpr int MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - BASE_HEADER_SIZE;
	static constexpr int MAX_FRAGMENT_PAYLOAD_SIZE = MAX_PACKET_SIZE - BASE_HEADER_SIZE - FRAGMENT_SUB_HEADER_SIZE;
	static constexpr int MAX_FRAGMENTS = 256;
	static constexpr int MAX_FRAGMENT_PACKET_BUFFER_SIZE = 512; // TODO: Rename

	enum class PacketType
	{
		REGULAR = 0,
		FRAGMENT = 1
	};

	struct BasePacketHeader
	{
		uint16_t sequence;
		PacketType packetType;
	};

	struct FragmentPacketSubHeader
	{
		uint8_t fragmentCount;
		uint8_t fragmentIndex;
	};

	struct ReceivedFragmentPacketData
	{
		uint32_t sequence;
		uint8_t fragmentCount;
		uint8_t fragmentReceivedCount;
		uint8_t* packetData;
		uint8_t fragmentReceived[MAX_FRAGMENTS];
		int totalSize;
	};

	static void WriteBasePacketHeader(uint8_t* packetBuffer, const BasePacketHeader* header);
	/**
	 * \brief  Only writes the fragment sub header, not the base header.
	 * \param packetBuffer Pointer into the packet buffer where the fragment sub header starts (directly after the base header).
	 */
	static void WriteFragmentPacketHeader(uint8_t* packetBuffer, const FragmentPacketSubHeader* header);
	static BasePacketHeader ReadBasePacketHeader(const uint8_t* packetBuffer);
	/**
	 * \brief  Only reads the fragment sub header, not the base header.
	 * \param packetBuffer Pointer into the packet buffer where the fragment sub header starts (directly after the base header).
	 */
	static FragmentPacketSubHeader ReadFragmentPacketSubHeader(const uint8_t* packetBuffer);

	void HandlePacket(uint8_t* packet, int packetSize);
	void InitNewFragmentPacketData(uint16_t sequence, uint8_t fragmentCount);

	uint16_t nextSequence = 0;
	ReceivedFragmentPacketData receivedFragmentPackets[MAX_FRAGMENT_PACKET_BUFFER_SIZE];
	Socket socket;
};
}
