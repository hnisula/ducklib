#include <iostream>

#include "render_util.h"
#include "../include/ducklib/render/result.h"

namespace ducklib::render {
void log(LogLevel level, Result status, std::string_view message) {
    auto status_message = to_string(status);
    std::cerr << to_string(level) << ": " << message << " (" << status_message << ")" << "\n";
    std::cerr.flush();
}
}
