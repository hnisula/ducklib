#ifndef APP_WINDOW_H
#define APP_WINDOW_H
#include <string_view>

namespace ducklib {
class AppWindow {
public:
    virtual ~AppWindow() {}

    virtual auto process_messages() -> void = 0;
    virtual auto close() -> void = 0;
    virtual auto is_open() const -> bool = 0;
};
}

#endif //APP_WINDOW_H
