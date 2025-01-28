#include "net/net_buffer_writer.h"

#include <cstring>
#include <cstdint>
#include <stdexcept>

namespace ducklib::net {
NetBufferWriter::NetBufferWriter(std::span<std::byte> buffer) {
    this->buffer = buffer;
}

auto NetBufferWriter::write(std::span<const std::byte> data_buffer) -> void {
    if (!can_write(data_buffer.size_bytes())) {
        throw std::runtime_error("Insufficient space in net buffer for write");
    }

    memcpy(buffer.data() + byte_position, data_buffer.data(), data_buffer.size_bytes());
    byte_position += data_buffer.size_bytes();
}

auto NetBufferWriter::bytes_written() const -> size_t {
    return byte_position;
}

auto NetBufferWriter::remaining_bytes() const -> size_t {
    return buffer.size_bytes() - byte_position;
}

auto NetBufferWriter::data() const -> std::span<const std::byte> {
    return { buffer.data(), bytes_written() };
}


auto NetBufferWriter::can_write(size_t bytes_to_write) const -> bool {
    return remaining_bytes() >= bytes_to_write;
}
}
