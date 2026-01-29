#ifdef __unix__
#include <array>
#include <cstdio>
#include <GLFW/glfw3.h>

#include "ducklib/core/shared.h"

namespace ducklib {
void glfw_error_callback(int error, const char* description) {
    std::array<char, 1024> buffer{};
    snprintf(buffer.data(), buffer.size(), "%s (%d)", description, error);
    core::log_callback(LogLevel::ERROR, buffer.data());
    return;
}

void init_platform() {
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
}

void terminate_platform() {
    glfwTerminate();
}
}
#endif
