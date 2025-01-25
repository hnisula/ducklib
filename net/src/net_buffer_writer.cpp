#include "net/net_buffer_writer.h"

#include <cstring>
#include <cstdint>

namespace ducklib::net {
NetBufferWriter::NetBufferWriter(std::span<std::byte> buffer) {
    this->buffer = buffer;
}

auto NetBufferWriter::write(std::span<std::byte> data_buffer) -> void {
    if (!can_write(data_buffer.size_bytes())) {
        return; // TODO: Handle errors!
    }

    memcpy(buffer.data() + byte_position, data_buffer.data(), data_buffer.size_bytes());
    byte_position += data_buffer.size_bytes();
}

auto NetBufferWriter::bytes_written() const -> size_t {
    return byte_position;
}

auto NetBufferWriter::can_write(size_t bytes) const -> bool {
    return bytes < buffer.size_bytes() - byte_position;
}
}
