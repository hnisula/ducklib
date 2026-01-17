#ifndef APP_WINDOW_H
#define APP_WINDOW_H
#include <functional>
#include <cstdint>

namespace ducklib {
class AppWindow {
public:
    enum class Type {
        WINDOWS
    };
    
    virtual ~AppWindow() {}

    virtual void process_messages() = 0;
    virtual void close() = 0;
    virtual bool is_open() const = 0;
    virtual Type type() const = 0;

    virtual uint32_t border_size() const = 0;
    virtual uint32_t title_bar_height() const = 0;
};
}

#endif //APP_WINDOW_H
