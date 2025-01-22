#include <cstring>

#include "net/net_client2.h"

#include <stdexcept>

namespace ducklib::net {
template <typename T>
auto write_buffer_data(std::span<std::byte> buffer, T value) -> std::span<std::byte>;
auto write_buffer_data(std::span<std::byte> buffer, const std::byte* data, int data_size) -> std::span<std::byte>;

auto NetClient2::send_packet(Address to, std::span<std::byte> data) -> int {
    if (data.size() > MAX_PAYLOAD_SIZE)
        throw std::runtime_error("Data size exceeds maximum payload size");
    // return send_fragment_packets(to, data);

    auto packet_buffer = std::array<std::byte, MAX_PACKET_SIZE>{};
    auto header = PacketHeader{ 0, PacketType::REGULAR };
    std::span<std::byte> remaining_buffer(packet_buffer);

    remaining_buffer = header.serialize(remaining_buffer);
    remaining_buffer = write_buffer_data(remaining_buffer, data.data(), static_cast<int>(data.size()));

    auto bytes_written = MAX_PACKET_SIZE - remaining_buffer.size();
    auto bytes_sent = socket.send(to, std::span(packet_buffer.data(), bytes_written));

    return bytes_sent;;
}

auto NetClient2::PacketHeader::serialize(std::span<std::byte> packet_buffer) const -> std::span<std::byte> {
    auto ordered_sequence = htons(sequence);
    auto ordered_packet_type = htons(static_cast<uint16_t>(packet_type));
    auto remaining_buffer = packet_buffer;

    remaining_buffer = write_buffer_data(remaining_buffer, ordered_sequence);
    remaining_buffer = write_buffer_data(remaining_buffer, ordered_packet_type);

    return remaining_buffer;
}

template <typename T>
auto write_buffer_data(std::span<std::byte> buffer, T value) -> std::span<std::byte> {
    return write_buffer_data(buffer, reinterpret_cast<std::byte*>(&value), sizeof(value));
}

auto write_buffer_data(std::span<std::byte> buffer, const std::byte* data, int data_size) -> std::span<std::byte> {
    if (data_size > buffer.size_bytes()) {
        return std::span<std::byte>(); // TODO: Handle errors better
    }

    std::memcpy(buffer.data(), data, data_size);
    return buffer.subspan(data_size);
}
}
