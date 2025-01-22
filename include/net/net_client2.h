#ifndef NETCLIENT2_H
#define NETCLIENT2_H
#include <span>
#include <net/socket.h>

namespace ducklib::net
{
    class NetClient2
    {
    public:
        auto send_packet(Address to, std::span<std::byte> data) -> int;
        auto receive_packet() -> bool;

    private:
        enum class PacketType
        {
            REGULAR = 0,
            FRAGMENT = 1
        };

        struct PacketHeader
        {
            uint16_t sequence;
            PacketType packet_type;

            auto serialize(std::span<std::byte> packet_buffer) const -> std::span<std::byte>;
            static auto deserialize(std::span<std::byte> packet_buffer) -> PacketHeader;
        };

        auto send_fragment_packets(Address to, std::span<std::byte> data) -> bool;

        static constexpr int MAX_PACKET_SIZE = 1024;
        static constexpr int MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE; // TODO: Subtract header size

        Socket socket;
    };
}

#endif //NETCLIENT2_H
