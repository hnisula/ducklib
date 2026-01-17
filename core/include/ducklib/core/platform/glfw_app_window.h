#ifndef DUCKLIB_GLFW_APP_WINDOW_H
#define DUCKLIB_GLFW_APP_WINDOW_H

#include <GLFW/glfw3.h>
#include <string_view>

#include "app_window.h"

namespace ducklib {
class GlfwAppWindow : public AppWindow {
    GlfwAppWindow(std::string_view title, uint32_t width, uint32_t height);
    ~GlfwAppWindow() override;
};
}

#endif //DUCKLIB_GLFW_APP_WINDOW_H