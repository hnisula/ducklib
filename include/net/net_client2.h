#ifndef NETCLIENT2_H
#define NETCLIENT2_H
#include <span>
#include <net/socket.h>

namespace ducklib::net
{
class NetClient2
{
public:
    auto send_packet(Address to, std::span<std::byte> data) -> bool;
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
    };

    auto send_fragment_packets(Address to, std::span<std::byte> data) -> bool;

    static auto write_regular_packet_header(
        std::span<std::byte> packet_buffer,
        const PacketHeader* header) -> std::span<std::byte>;

    static auto write_buffer_data(std::byte* buffer, const std::byte* data, int data_size) -> std::byte*;
    template <typename T>
    static auto write_buffer_data(std::byte* buffer, T) -> std::byte*;

    static constexpr int MAX_PACKET_SIZE = 1024;
    static constexpr int MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE; // TODO: Subtract header size

    Socket socket;
};

template <typename T>
auto NetClient2::write_buffer_data(std::byte* buffer, T value) -> std::byte*
{
    static_assert(std::is_trivially_copyable_v<T>);

    return write_buffer_data(buffer, reinterpret_cast<std::byte*>(&value), sizeof(T));
}
}

#endif //NETCLIENT2_H
