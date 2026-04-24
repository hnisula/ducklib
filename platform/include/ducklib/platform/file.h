#pragma once
#include <cstdint>
#include <span>
#include <string_view>
#define _FILE_OFFSET_BITS=64
#define _LARGEFILE_SOURCE
#include <stdio.h>

namespace ducklib {
enum class FileMode {
    READ,
    WRITE
};

enum class SeekOrigin {
    BEGINNING,
    CURSOR,
    END
};

class File {
public:
    void open(std::string_view filename, FileMode mode);
    void read_all(std::span<std::byte> buffer);
    void seek(uint64_t offset, SeekOrigin origin);
    uint64_t tell();
    uint64_t size();
    void close();

private:
    static const char* map_file_mode(FileMode mode);
    static int map_seek_origin(SeekOrigin origin);
    FILE* file = nullptr;
};
}
