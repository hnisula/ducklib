#ifndef NETCLIENT2_H
#define NETCLIENT2_H
#include <span>
#include <net/socket.h>

#include "net_buffer_reader.h"
#include "net_buffer_writer.h"

namespace ducklib::net
{
    class NetClient
    {
    public:
        auto send_packet(Address to, std::span<std::byte> data) -> size_t;
        auto receive_packet(std::span<std::byte> dest_buffer, Address& from) -> size_t;

    private:
        enum class PacketType : uint16_t
        {
            REGULAR = 0,
            FRAGMENT = 1
        };

        struct PacketHeader
        {
            uint16_t sequence;
            PacketType packet_type;

            auto serialize(NetBufferWriter buffer_writer) const -> void;
            static auto deserialize(NetBufferReader buffer_reader) -> PacketHeader;
        };

        auto send_regular_packet(Address to, std::span<const std::byte> data) -> size_t;
        auto send_fragment_packets(Address to, std::span<std::byte> data) -> bool;

        auto receive_regular_packet(NetBufferReader packet_reader, std::span<std::byte> dest_buffer) -> size_t;

        static constexpr int MAX_PACKET_SIZE = 1024;
        static constexpr int MAX_REGULAR_PAYLOAD_SIZE = MAX_PACKET_SIZE - sizeof(PacketHeader);

        Socket socket;

        uint64_t total_bytes_sent = 0;
        uint64_t total_bytes_received = 0;
    };
}

#endif //NETCLIENT2_H
