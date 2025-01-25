#ifndef BUFFER_WRITER_H
#define BUFFER_WRITER_H
#include <cstdint>
#include <functional>
#include <span>

#include "net.h"

namespace ducklib::net {
class NetBufferWriter {
public:
    explicit NetBufferWriter(std::span<std::byte> buffer);
    
    template <typename T>
    auto write(T value) -> void;
    auto write(std::span<std::byte> data_buffer) -> void;

    [[nodiscard]]
    auto bytes_written() const -> size_t;

private:
    std::span<std::byte> buffer;
    std::size_t byte_position = 0;

    [[nodiscard]]
    auto can_write(size_t bytes) const -> bool;
};

template <typename T>
auto NetBufferWriter::write(T value) -> void {
    auto value_net = host_to_net(value);
    write({ reinterpret_cast<std::byte*>(&value_net), sizeof(int16_t) });
}
}

#endif //BUFFER_WRITER_H
