#ifndef DUCKLIB_VK_H
#define DUCKLIB_VK_H
#ifdef DL_VK

#include <vulkan/vulkan.h>

#include "../result.h"
#include "shared.h"

namespace ducklib::render {
struct DescriptorHeap {
    VkDescriptorPool vk_descriptor_pool;
};

struct CommandQueue {
    VkQueue vk_queue;
    QueueType type;
};

struct Device {
    VkDevice vk_device;
    VkPhysicalDevice vk_adapter;
    CommandQueue graphics_queue;
    CommandQueue copy_queue;
    
    void create_queue(QueueType type, CommandQueue& out_queue);
};

struct Rhi;
void create_rhi(Rhi& out_rhi);
void destroy_rhi(Rhi& rhi);

struct Rhi {
    VkInstance vk_instance;

    // When out_adapters is nullptr, adapter_count will receive the number of adapters available.
    // When out_adapters is set, it will be used as the max count of out_adapters.
    Result enumerate_adapters(uint32_t& adapter_count, AdapterInfo* out_adapters) const;
    Result create_device(const AdapterInfo& adapter, Device& out_device) const;
    
    static constexpr uint32_t MAX_VK_ADAPTERS = 32;
};
}

#endif
#endif