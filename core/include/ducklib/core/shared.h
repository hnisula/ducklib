#ifndef DUCKLIB_SHARED_H
#define DUCKLIB_SHARED_H
#include <string_view>

#include "logging/log_level.h"

namespace ducklib::core {
extern void (*log)(LogLevel level, std::string_view message);
}
#endif //DUCKLIB_SHARED_H