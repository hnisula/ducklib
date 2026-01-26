#ifndef DUCKLIB_SHARED_H
#define DUCKLIB_SHARED_H
#include <string_view>
#include <functional>

#include "logging/log_level.h"

namespace ducklib::core {
extern std::function<void(LogLevel level, std::string_view message)> log_callback;
}
#endif //DUCKLIB_SHARED_H