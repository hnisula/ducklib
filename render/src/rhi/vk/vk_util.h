#ifndef DUCKLIB_VK_UTIL_H
#define DUCKLIB_VK_UTIL_H
#include <vulkan/vulkan_core.h>

#include "../../render_util.h"
#include "ducklib/render/result.h"
#include "ducklib/render/rhi/shared.h"

namespace ducklib::render {
Result map_vk_result(VkResult vk_result);
AdapterType map_vk_adapter_type(VkPhysicalDeviceType vk_type);

#define VK_CHECK(expr, err_msg) \
    do { \
        auto vk_error = (expr); \
        if (vk_error != VK_SUCCESS) { \
            auto dl_error = map_vk_result(vk_error); \
            render_log(LogLevel::ERROR, dl_error, err_msg); \
            return dl_error; \
        } \
    } while (false)
#define VK_CHECK_V(expr, err_msg) \
    do { \
        auto vk_error = (expr); \
        if (vk_error != VK_SUCCESS) { \
            auto dl_error = map_vk_result(vk_error); \
            render_log(LogLevel::ERROR, dl_error, err_msg); \
            return; \
        } \
    } while (false)
}

#endif //DUCKLIB_VK_UTIL_H
