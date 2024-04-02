#include <cstring>

#include "net/net_client2.h"

namespace ducklib::net
{
auto NetClient2::send_packet(Address to, std::span<std::byte> data) -> bool
{
    if (data.size() > MAX_PAYLOAD_SIZE)
        return send_fragment_packets(to, data);
    
    write_regular_packet_header()

    return true;
}

auto NetClient2::write_regular_packet_header(
    std::span<std::byte> packet_buffer,
    const PacketHeader* header) -> std::span<std::byte>
{
    auto sequence = htons(header->sequence);
    auto packet_type = htons(static_cast<uint16_t>(header->packet_type));
    auto* buffer_ptr = packet_buffer.data();

    buffer_ptr = write_buffer_data(buffer_ptr, sequence);
    buffer_ptr = write_buffer_data(buffer_ptr, packet_type);

    return packet_buffer.subspan(buffer_ptr - packet_buffer.data());
}

auto NetClient2::write_buffer_data(
    std::byte* buffer,
    const std::byte* data,
    int data_size) -> std::byte*
{
    std::memcpy(buffer, data, data_size);
    return buffer + data_size;
}
}
