#ifndef WIN_APP_WINDOW_H
#define WIN_APP_WINDOW_H
#include <string_view>

#ifdef _WIN32
#include <Windows.h>

#include "ducklib/core/app_window.h"

namespace ducklib {
class WinAppWindow final : AppWindow {
public:
    WinAppWindow(std::string_view title, int width, int height);
    ~WinAppWindow() override;

    void process_messages() override;
    void close() override;
    bool is_open() const override;
    Type type() const override;

    void register_message_callback(std::function<void(AppWindow*, uint32_t, WPARAM, LPARAM)> callback);
    void process_message_callbacks(uint32_t msg, WPARAM w_param, LPARAM l_param);

    HWND hwnd() const;
    uint32_t border_size() const override;
    uint32_t title_bar_height() const override;

private:
    HWND window_handle;
    std::vector<std::function<void(AppWindow*, uint32_t, WPARAM, LPARAM)>> message_callbacks;
    uint32_t caption_height;
    uint32_t frame_border_size;

    static bool class_initialized;
    static constexpr auto DEFAULT_WINDOW_CLASS_NAME = "WinAppWindow";

    static void register_window_class();
};
}

#endif
#endif