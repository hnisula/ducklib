#include "net/net_buffer_reader.h"

#include <cstring>
#include <stdexcept>

namespace ducklib::net {
NetBufferReader::NetBufferReader(std::span<const std::byte> buffer)
    : buffer(buffer) {}

auto NetBufferReader::read(std::span<std::byte> dest_buffer, size_t bytes_to_read) -> void {
    if (!can_read(bytes_to_read) || bytes_to_read > dest_buffer.size()) {
        throw std::invalid_argument("Insufficient data in net buffer for read");
    }

    memcpy(dest_buffer.data(), buffer.data() + byte_position, bytes_to_read);
    byte_position += bytes_to_read;
}

auto NetBufferReader::remaining_bytes() const -> size_t {
    return buffer.size_bytes() - byte_position;
}

auto NetBufferReader::can_read(size_t bytes_to_read) const -> bool {
    return remaining_bytes() > bytes_to_read;
}
}
