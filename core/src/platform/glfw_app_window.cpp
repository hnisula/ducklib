#ifdef __unix__
#include <array>
#include <cstdio>

#include "ducklib/core/shared.h"
#include "ducklib/core/platform/app_window.h"

namespace ducklib {
void glfw_error_callback(int error, const char* description) {
    std::array<char, 1024> buffer{};
    snprintf(buffer.data(), buffer.size(), "%s (%d)", description, error);
    core::log(LogLevel::ERROR, buffer.data());
    return;
}
AppWindow::AppWindow(std::string_view title, uint32_t width, uint32_t height) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_handle = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    
    if (!window_handle) {
        return;
    }
}

AppWindow::~AppWindow() {
    if (!window_handle) {
        glfwDestroyWindow(window_handle);
    }
}

void AppWindow::close() {
    glfwSetWindowShouldClose(window_handle, 1);
}

bool AppWindow::is_open() const {
    // TODO: Find out how to detect this but perhaps it's just to check the should close flag
    return true;
}
}
#endif