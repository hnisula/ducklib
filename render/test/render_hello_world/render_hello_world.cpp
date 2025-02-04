#include <Windows.h>
#include <memory>

#include "core/win/win_app_window.h"

using namespace ducklib;

int __stdcall WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int cmdShow) {
    auto window = std::make_unique<WinAppWindow>("Hello world!", 600, 400);

    while (window->is_open()) {
        window->process_messages();
    }
    
    return 0;
}
