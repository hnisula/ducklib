#include <iostream>

#include "ducklib/render/rhi/vk.h"
#include "../src/render_util.h"
#include "ducklib/core/logging/logger.h"
#include "ducklib/render/result.h"

using namespace ducklib;

Logger logger{};
void log(LogLevel level, render::Result status, std::string_view message) {
    auto status_message = to_string(status);
    std::array<char, 1024> buffer{};
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
    render::Rhi rhi{};
    render::Device device{};
    
    render::create_rhi(rhi);
    uint32_t adapter_count = 0;
    render::AdapterInfo adapters[32];
    CHECK(rhi.enumerate_adapters(adapter_count, nullptr), "Failed to enumerate rhi adapters");
    CHECK(rhi.enumerate_adapters(adapter_count, adapters), "Failed to enumerate rhi adapters");
    CHECK(rhi.create_device(adapters[0], device), "Failed to create rhi device");
    
    // device.create_queue();

    return 0;
}