#ifndef DUCKLIB_VK_H
#define DUCKLIB_VK_H
#ifdef DL_VK

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "../result.h"
#include "shared.h"

namespace ducklib::render {
struct Image {
    VkImage vk_image;
};

struct ImageView {
    Image* image;
    VkImageView vk_image_view;
};

struct DescriptorHeap {
    VkDescriptorPool vk_descriptor_pool;
};

struct CommandQueue {
    VkQueue vk_queue;
    QueueType type;
    uint32_t queue_family_index = 0U;
};

struct CommandList {
    VkCommandBuffer vk_cmd_buffer;
    VkCommandPool vk_cmd_pool;

    void clear_rt(Image image, ImageLayout current_image_layout, const float color_rgba[4]);
    
    void transition_barrier(Image image, ImageLayout current_layout, ImageLayout new_layout);
};

struct Device {
    VkDevice vk_device;
    VkPhysicalDevice vk_adapter;
    CommandQueue graphics_queue;
    CommandQueue compute_queue;
    CommandQueue copy_queue;

    Result create_command_list(QueueType queue_type, CommandList& out_command_list);
    Result create_image_view(VkImage image, Format format, ImageView& out_image_view);

    uint32_t get_queue_type_family_index(QueueType queue_type);
};

struct FrameSyncData {
    VkFence in_flight{};
    VkSemaphore image_available{};
    VkSemaphore render_completed{};
};

struct Swapchain {
    static constexpr auto MAX_IMAGE_COUNT = 4U;

    VkSwapchainKHR vk_swapchain = nullptr;
    VkSurfaceKHR vk_surface = nullptr;
    Device* device = nullptr;
    FrameSyncData sync_data[MAX_IMAGE_COUNT];
    // TODO: Use an Image wrapper type?
    Image images[MAX_IMAGE_COUNT];
    uint64_t frame_number = 0U;
    uint32_t buffer_count = 0U;
    uint32_t current_image_index = 0U;
    uint32_t width = 0;
    uint32_t height = 0;

    void acquire_next_image();
    void present();
    Image get_current_image();
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
    Result create_swapchain_glfw(
        Device& device,
        GLFWwindow* window_handle,
        uint32_t width,
        uint32_t height,
        Format format,
        uint8_t image_count,
        Swapchain& out_swapchain);
    void destroy_swapchain(Device& device, Swapchain& swapchain);

    static constexpr uint32_t MAX_VK_ADAPTERS = 32;
};
}

#endif
#endif
