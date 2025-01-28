#ifndef NET_BUFFER_READER_H
#define NET_BUFFER_READER_H
#include <functional>
#include <span>

#include "net.h"

namespace ducklib::net {
class NetBufferReader {
public:
    explicit NetBufferReader(std::span<const std::byte> buffer);

    template <typename T>
    auto read() -> T;
    auto read(std::span<std::byte> dest_buffer, size_t bytes_to_read) -> void;

    [[nodiscard]]
    auto remaining_bytes() const -> size_t;

private:
    std::span<const std::byte> buffer;
    std::size_t byte_position = 0;

    [[nodiscard]]
    auto can_read(size_t bytes_to_read) const -> bool;
};

template <typename T>
auto NetBufferReader::read() -> T {
    T value{};
    read(std::span{ reinterpret_cast<std::byte*>(&value), sizeof(T) }, sizeof(T));

    return net_to_host(value);
}
}

#endif //NET_BUFFER_READER_H
