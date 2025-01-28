#include <cstring>

#include <stdexcept>
#include <array>

#include "net/net_client.h"
#include "net/net_buffer_writer.h"

namespace ducklib::net {
auto NetClient::send_packet(Address to, std::span<std::byte> data) -> size_t {
    if (data.size() > MAX_REGULAR_PAYLOAD_SIZE) {
        throw std::runtime_error("Data size exceeds maximum regular payload size");
    } else {
        return send_regular_packet(to, data);
    }
}

auto NetClient::receive_packet(std::span<std::byte> dest_buffer, Address& from) -> size_t {
    std::array<std::byte, MAX_PACKET_SIZE> packet_buffer;
    auto bytes_read{ socket.receive(from, packet_buffer) };
    auto packet_reader{ NetBufferReader({ packet_buffer.data(), bytes_read }) };
    auto packet_header{ PacketHeader::deserialize(packet_reader) };
    auto bytes_received{ 0 };

    if (packet_header.packet_type == PacketType::REGULAR) {
        bytes_received = receive_regular_packet(packet_reader, dest_buffer);
    }

    total_bytes_received += bytes_received;
    return bytes_received;
}

auto NetClient::PacketHeader::serialize(NetBufferWriter buffer_writer) const -> void {
    buffer_writer.write(sequence);
    buffer_writer.write(static_cast<uint16_t>(packet_type));
}

auto NetClient::PacketHeader::deserialize(NetBufferReader buffer_reader) -> PacketHeader {
    auto sequence{ buffer_reader.read<uint16_t>() };
    auto packet_type{ buffer_reader.read<uint16_t>() };

    return PacketHeader{ sequence, static_cast<PacketType>(packet_type) };
}

auto NetClient::send_regular_packet(Address to, std::span<const std::byte> data) -> size_t {
    std::array<std::byte, MAX_PACKET_SIZE> packet_buffer;
    auto packet_writer{ NetBufferWriter{ packet_buffer } };

    packet_writer.write({ data.data(), data.size() });
    auto bytes_sent{ socket.send(to, packet_writer.data()) };
    total_bytes_sent += bytes_sent;

    return bytes_sent;
}

auto NetClient::receive_regular_packet(NetBufferReader packet_reader, std::span<std::byte> dest_buffer) -> size_t {
    if (dest_buffer.size_bytes() < packet_reader.remaining_bytes()) {
        throw std::runtime_error("Insufficient destination net buffer for read");
    }

    auto bytes_to_read{ packet_reader.remaining_bytes() };
    packet_reader.read(dest_buffer, bytes_to_read);
    total_bytes_received += bytes_to_read;

    return bytes_to_read;
}
}
