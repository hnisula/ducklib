#ifdef __unix__
#include "ducklib/core/platform/glfw_app_window.h"

namespace ducklib {
GlfwAppWindow::GlfwAppWindow(std::string_view title, uint32_t width, uint32_t height) {
    auto window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
}
}
#endif