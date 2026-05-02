#ifndef APP_WINDOW_H
#define APP_WINDOW_H
#include <cstdint>
#ifdef _WIN32
#include <Windows.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "ducklib/core/math.h"

namespace ducklib {
class AppWindow {
public:
    AppWindow(std::string_view title, uint32_t width, uint32_t height);
    ~AppWindow();

    void process_messages();
    void close();
    bool is_open() const;

    Rect get_client_area() const;
    uint32_t client_width() const;
    uint32_t client_height() const;
    
    void* handle() const { return window_handle; }

private:
#if defined(_WIN32)
    HWND window_handle;
#else
    GLFWwindow* window_handle;
#endif
    uint32_t width;
    uint32_t height;
};
}

#endif //APP_WINDOW_H
