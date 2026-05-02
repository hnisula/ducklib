#include "ducklib/platform/file.h"

namespace ducklib {
File File::open(std::string_view filename, FileMode mode) {
#ifdef __unix__
    auto handle = fopen(filename.data(), map_file_mode(mode));
    File file;
    file.handle = handle;
    return file;
#endif
}

uint64_t File::read_all(std::span<std::byte> buffer) {
    auto file_size = size();
    return fread(buffer.data(), sizeof(std::byte), file_size, handle);
}

void File::seek(uint64_t offset, SeekOrigin origin) {
#ifdef __unix__
    fseeko(handle, offset, map_seek_origin(origin));
#endif
}

uint64_t File::tell() {
#ifdef __unix__
    return ftello(handle);
#endif
}

uint64_t File::size() {
    auto last_pos = this->tell();
    this->seek(0, SeekOrigin::END);
    auto size = this->tell();
    this->seek(last_pos, SeekOrigin::BEGINNING);
    return size;
}

void File::close() {
    if (handle) {
        fclose(handle);
        handle = nullptr;
    }
}

const char* File::map_file_mode(FileMode mode) {
    switch (mode) {
    case FileMode::READ: return "r";
    case FileMode::WRITE: return "w";
    default: return nullptr;
    }
}

int File::map_seek_origin(SeekOrigin origin) {
    switch (origin) {
    case SeekOrigin::BEGINNING: return SEEK_SET;
    case SeekOrigin::CURSOR: return SEEK_CUR;
    case SeekOrigin::END: return SEEK_END;
    default: return -1;
    }
}
}
