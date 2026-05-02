#include "ducklib/core/shared.h"

namespace ducklib::core {
std::function<void(LogLevel level, std::string_view message)> log_callback;
}