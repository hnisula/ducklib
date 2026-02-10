#include <iostream>
#include <functional>

#include "ducklib/core/shared.h"
#include "ducklib/render/rhi/vk.h"
#include "../src/render_util.h"
#include "ducklib/core/logging/logger.h"
#include "ducklib/core/platform/app_window.h"
#include "ducklib/core/platform/platform.h"
#include "ducklib/render/result.h"

using namespace ducklib;

Logger logger{};

void log(LogLevel level, render::Result status, std::string_view message) {
    auto status_message = to_string(status);
    // std::array<char, 1024> buffer{};
    std::cerr << to_string(level) << ": " << message << "(" << status_message << ")" << "\n";
    std::cerr.flush();
}

#define CHECK(expr, err_msg) \
    do { \
        auto result = (expr); \
        if (result != render::Result::SUCCESS) { \
            log(LogLevel::ERROR, result, err_msg); \
            return 0; \
    } \
    } while (false)

int main() {
    init_platform();
    auto running = true;

    core::log_callback = [](LogLevel level, std::string_view message) {
        logger.log(level, message);
    };
    render::Rhi rhi{};
    render::Device device{};
    render::Swapchain swapchain{};
    AppWindow window{"glfw-vk-test", 800, 600};

    render::create_rhi(rhi);
    uint32_t adapter_count = 0;
    render::AdapterInfo adapters[32];
    CHECK(rhi.enumerate_adapters(adapter_count, nullptr), "Failed to enumerate rhi adapters");
    CHECK(rhi.enumerate_adapters(adapter_count, adapters), "Failed to enumerate rhi adapters");
    CHECK(rhi.create_device(adapters[0], device), "Failed to create rhi device");
    CHECK(
        rhi.create_swapchain_glfw(device, (GLFWwindow*)window.handle(), window.client_width(), window.client_height(),
            render::Format::B8G8R8A8_UNORM, swapchain), "Failed to create swap chain");

    while (running) {
        window.process_messages();
    }

    terminate_platform();
    return 0;
}
