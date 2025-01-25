#include <cstring>

#include "net/net_client2.h"

#include <stdexcept>

#include "net/net_buffer_writer.h"

namespace ducklib::net {
auto NetClient2::send_packet(Address to, std::span<std::byte> data) -> int {
    if (data.size() > MAX_PAYLOAD_SIZE)
        throw std::runtime_error("Data size exceeds maximum payload size");
    // return send_fragment_packets(to, data);

    auto packet_buffer = std::array<std::byte, MAX_PACKET_SIZE>{};
    auto header = PacketHeader{ .sequence = 0, .packet_type = PacketType::REGULAR };
    auto buffer_writer = NetBufferWriter{ packet_buffer };

    header.serialize(buffer_writer);
    buffer_writer.write({ data.data(), data.size() });

    auto bytes_sent = socket.send(to, { packet_buffer.data(), buffer_writer.bytes_written() });

    return bytes_sent;
}

auto NetClient2::PacketHeader::serialize(NetBufferWriter buffer_writer) const -> void {
    buffer_writer.write(sequence);
    buffer_writer.write(packet_type);
}
}
