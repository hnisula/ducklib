#include <iostream>
#include <unistd.h>

#include "ducklib/core/shared.h"
#include "ducklib/render/rhi/vk/vk.h"
#include "../src/render_util.h"
#include "ducklib/core/logging/logger.h"
#include "ducklib/core/platform/app_window.h"
#include "ducklib/core/platform/platform.h"
#include "ducklib/render/result.h"

using namespace ducklib;

Logger logger{};

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
    auto width = 800U;
    auto height = 600U;
    render::Rhi rhi{};
    render::Device device{};
    render::Swapchain swapchain{};
    Rect render_area{ 0, 0, width, height };
    AppWindow window{ "glfw-vk-test", width, height };
    render::CommandList cmd_list{};

    render::create_rhi(rhi);
    uint32_t adapter_count = 0;
    render::AdapterInfo adapters[32];
    CHECK(rhi.enumerate_adapters(adapter_count, nullptr), "Failed to enumerate rhi adapters");
    CHECK(rhi.enumerate_adapters(adapter_count, adapters), "Failed to enumerate rhi adapters");
    CHECK(rhi.create_device(adapters[0], device), "Failed to create rhi device");
    CHECK(
        rhi.create_swapchain_glfw(device, (GLFWwindow*)window.handle(), window.client_width(), window.client_height(),
            render::Format::B8G8R8A8_UNORM, 2, swapchain),
        "Failed to create swap chain");
    CHECK(device.create_command_list(render::QueueType::GRAPHICS, cmd_list), "Failed to create command buffer");

    while (running) {
        window.process_messages();

        constexpr render::Color clear_color = { 0.0f, 0.2f, 0.4f, 1.0f };

        auto sync = swapchain.get_current_sync();
        sync.wait_for_render();
        swapchain.acquire_next_image();
        cmd_list.open();
        render::Attachment attachments = {
            swapchain.get_current_image_view(),
            render::ImageLayout::COLOR_ATTACHMENT,
            render::LoadOp::CLEAR,
            render::StoreOp::NONE,
            clear_color
        };
        cmd_list.transition_barrier(swapchain.get_current_image(),
                                    render::ImageLayout::UNDEFINED,
                                    render::ImageLayout::COLOR_ATTACHMENT);
        cmd_list.begin_render(render_area, &attachments, 1);
        // Do stuff later
        cmd_list.end_render();
        cmd_list.transition_barrier(swapchain.get_current_image(),
                                    render::ImageLayout::COLOR_ATTACHMENT,
                                    render::ImageLayout::PRESENT_SRC);
        cmd_list.close();
        device.graphics_queue.submit(cmd_list, swapchain.get_current_sync());
        swapchain.present();
        sleep(20);
    }

    terminate_platform();
    return 0;
}
