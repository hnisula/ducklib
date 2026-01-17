#ifndef DUCKLIB_RENDER_UTIL_H
#define DUCKLIB_RENDER_UTIL_H
#include <string_view>

#include "ducklib/core/logging/log_level.h"
#include "ducklib/render/result.h"

namespace ducklib::render {
void render_log(LogLevel level, Result result, std::string_view message);
}

#endif //DUCKLIB_RENDER_UTIL_H