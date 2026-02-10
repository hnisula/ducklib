#ifdef __unix__

#include "ducklib/core/shared.h"
#include "ducklib/core/platform/app_window.h"

namespace ducklib {
AppWindow::AppWindow(std::string_view title, uint32_t width, uint32_t height) 
    : width(width), height(height) {
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

void AppWindow::process_messages() {
    glfwPollEvents();
}

void AppWindow::close() {
    glfwSetWindowShouldClose(window_handle, 1);
}

bool AppWindow::is_open() const {
    // TODO: Find out how to detect this but perhaps it's just to check the should close flag
    return true;
}

uint32_t AppWindow::client_width() const {
    // TODO: Fix these
    return width;
}

uint32_t AppWindow::client_height() const {
    return height;
}
}
#endif