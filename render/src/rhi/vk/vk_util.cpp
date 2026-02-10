#include "vk_util.h"

namespace ducklib::render {
Result map_vk_result(VkResult vk_result) {
    switch (vk_result) {
    case VK_SUCCESS: return Result::SUCCESS;
    case VK_INCOMPLETE: return Result::INSUFFICIENT_SPACE;
    case VK_ERROR_EXTENSION_NOT_PRESENT: return Result::EXTENSION_NOT_FOUND;
    default: return Result::ERROR;
    }
}

AdapterType map_vk_adapter_type(VkPhysicalDeviceType vk_type) {
    switch (vk_type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return AdapterType::DISCRETE_GPU;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return AdapterType::INTEGRATED_GPU;
    case VK_PHYSICAL_DEVICE_TYPE_CPU: return AdapterType::CPU;
    default: return AdapterType::OTHER;
    }
}
}
