// #ifdef DL_VK

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cstring>

#include "vk_util.h"
#include "ducklib/render/rhi/vk.h"

#include <GLFW/glfw3.h>

namespace ducklib::render {
constexpr auto MAX_QUEUE_FAMILIES = 32U;

struct QueueFamilyIndices {
    uint32_t graphics_index = -1;
    uint32_t compute_index = -1;
    uint32_t copy_index = -1;
};

QueueFamilyIndices get_supported_queue_indices(VkPhysicalDevice vk_device) {
    VkQueueFamilyProperties2 family_prop_sets[MAX_QUEUE_FAMILIES]{};
    uint32_t family_count = 0;

    for (auto& family_props : family_prop_sets) {
        family_props.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    }

    vkGetPhysicalDeviceQueueFamilyProperties2(vk_device, &family_count, nullptr);
    assert(family_count <= MAX_QUEUE_FAMILIES);
    vkGetPhysicalDeviceQueueFamilyProperties2(vk_device, &family_count, family_prop_sets);

    QueueFamilyIndices family_indices{};

    for (auto i = 0U; i < family_count; ++i) {
        auto flags = family_prop_sets[i].queueFamilyProperties.queueFlags;
        auto graphics = flags & VK_QUEUE_GRAPHICS_BIT;
        auto compute = flags & VK_QUEUE_COMPUTE_BIT;
        auto copy = flags & VK_QUEUE_TRANSFER_BIT;

        if (family_indices.graphics_index == -1U && graphics && compute && copy) {
            family_indices.graphics_index = i;
        }

        if (family_indices.compute_index == -1U && !graphics && compute && copy) {
            family_indices.compute_index = i;
        }

        if (family_indices.copy_index == -1U && !graphics && !compute && copy) {
            family_indices.copy_index = i;
        }
    }

    assert(family_indices.graphics_index != -1U);
    assert(family_indices.compute_index != -1U);
    assert(family_indices.copy_index != -1U);

    return family_indices;
}

void create_rhi(Rhi& out_rhi) {
    VkApplicationInfo app_info{};
    app_info.apiVersion = VK_API_VERSION_1_4;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "fixme";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "fixme";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info{};
    const char* validation_layers[] = { "VK_LAYER_KHRONOS_validation" };
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

#ifdef _WIN32
    const char *win_extensions[] = "VK_KHR_surface", "VK_KHR_win32_surface"
    create_info.enabledExtensionCount = 2;
    create_info.ppEnabledExtensionNames = win_extensions;
#else
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = glfw_extensions;
#endif

#ifdef NDEBUG
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = nullptr;
#else
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = validation_layers;
#endif

    if (vkCreateInstance(&create_info, nullptr, &out_rhi.vk_instance)) {
        throw std::runtime_error("Failed to create vk instance");
    }
}

void Fence::wait() {
    uint64_t wait_value = counter;
    VkSemaphoreWaitInfo wait_info{};
    wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    wait_info.semaphoreCount = 1;
    wait_info.pSemaphores = &vk_semaphore;
    wait_info.pValues = &wait_value;
    VK_CHECK_V(vkWaitSemaphores(vk_device, &wait_info, UINT64_MAX), "Failed to wait for vk semaphore");
}

void CommandList::open() {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK_V(vkBeginCommandBuffer(vk_cmd_buffer, &begin_info), "Failed to begin vk command buffer");
}

void CommandList::close() {
    VK_CHECK_V(vkEndCommandBuffer(vk_cmd_buffer), "Failed to end vk command buffer");
}

void CommandList::begin_render(Rect render_area, const Attachment* attachments, uint32_t attachment_count) {
    assert(attachment_count <= Attachment::MAX_ATTACHMENT_COUNT);
    VkRenderingAttachmentInfo color_attachment[Attachment::MAX_ATTACHMENT_COUNT]{};

    for (auto i = 0U; i < attachment_count; ++i) {
        auto& clear_color = attachments[i].clear_color;
        color_attachment[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment[i].imageView = attachments[i].image_view.vk_image_view;
        color_attachment[i].imageLayout = map_vk_image_layout(attachments[i].image_layout);
        color_attachment[i].loadOp = map_vk_load_op(attachments[i].load_op);
        color_attachment[i].storeOp = map_vk_store_op(attachments[i].store_op);
        color_attachment[i].clearValue = { { { clear_color.r, clear_color.g, clear_color.b, clear_color.a } } };
    }

    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea = { { render_area.x, render_area.y }, { render_area.width, render_area.height } };
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = attachment_count;
    render_info.pColorAttachments = color_attachment;
    vkCmdBeginRendering(vk_cmd_buffer, &render_info);
}

void CommandList::end_render() {
    vkCmdEndRendering(vk_cmd_buffer);
}

void CommandList::clear_rt(Image image, ImageLayout current_image_layout, Color color_rgba) {
    auto vk_current_image_layout = map_vk_image_layout(current_image_layout);
    VkClearColorValue vk_color{ { color_rgba.r, color_rgba.g, color_rgba.b, color_rgba.a } };
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;
    vkCmdClearColorImage(vk_cmd_buffer, image.vk_image, vk_current_image_layout, &vk_color, 1, &range);
}

void CommandList::transition_barrier(Image image, ImageLayout current_layout, ImageLayout new_layout) {
    auto vk_current_layout = map_vk_image_layout(current_layout);
    auto vk_new_layout = map_vk_image_layout(new_layout);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = vk_current_layout;
    barrier.newLayout = vk_new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.vk_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(
        vk_cmd_buffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);
}

void FrameSyncData::wait_for_render() {
    VK_CHECK_V(vkWaitForFences(vk_device, 1, &in_flight, VK_TRUE, UINT64_MAX), "Failed to wait for swapchain fence");
}

void CommandQueue::submit(CommandList& list, FrameSyncData& swapchain_sync_data) {
    VkCommandBufferSubmitInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    buffer_info.deviceMask = 1;
    buffer_info.commandBuffer = list.vk_cmd_buffer;
    VkSemaphoreSubmitInfo wait_semaphore_info{};
    wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info.deviceIndex = 0;
    wait_semaphore_info.semaphore = swapchain_sync_data.image_available;
    wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSemaphoreSubmitInfo finish_semaphore_info{};
    finish_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    finish_semaphore_info.deviceIndex = 0;
    finish_semaphore_info.semaphore = swapchain_sync_data.render_completed;
    finish_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    VkSubmitInfo2 submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount = 1;
    submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &finish_semaphore_info;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &buffer_info;
    VK_CHECK_V(vkQueueSubmit2(vk_queue, 1, &submit_info, swapchain_sync_data.in_flight),
               "Failed to submit vk command buffer to queue");
}

Result Device::create_command_list(QueueType queue_type, CommandList& out_command_list) {
    auto queue_family_index = get_queue_type_family_index(queue_type);
    VkCommandPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = queue_family_index;
    VK_CHECK(
        vkCreateCommandPool(vk_device, &pool_create_info, nullptr, &out_command_list.vk_cmd_pool),
        "Failed to create vk command pool");

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = out_command_list.vk_cmd_pool;
    alloc_info.commandBufferCount = 1;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    VK_CHECK(vkAllocateCommandBuffers(vk_device, &alloc_info, &out_command_list.vk_cmd_buffer),
             "Failed to create vk command list");

    return Result::SUCCESS;
}

Result Device::create_image_view(const Image& image, Format format, ImageView& out_image_view) {
    auto vk_format = map_vk_format(format);
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.format = vk_format;
    create_info.image = image.vk_image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    VK_CHECK(vkCreateImageView(vk_device, &create_info, nullptr, &out_image_view.vk_image_view),
             "Failed to create vk image view");

    return Result::SUCCESS;
}

uint32_t Device::get_queue_type_family_index(QueueType queue_type) {
    switch (queue_type) {
    case QueueType::GRAPHICS: return graphics_queue.queue_family_index;
    case QueueType::COMPUTE: return compute_queue.queue_family_index;
    case QueueType::COPY: return copy_queue.queue_family_index;
    default: return -1; // TODO: Better handling of invalid values
    }
}

void Swapchain::acquire_next_image() {
    auto sync_primitives = get_current_sync();
    VkAcquireNextImageInfoKHR acquire_info{};
    acquire_info.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
    acquire_info.swapchain = vk_swapchain;
    acquire_info.timeout = UINT64_MAX;
    acquire_info.deviceMask = 1;
    acquire_info.semaphore = sync_primitives.image_available;
    VK_CHECK_V(vkAcquireNextImage2KHR(device->vk_device, &acquire_info, &current_image_index),
               "Failed to acquire vk swapchain image");
    VK_CHECK_V(vkResetFences(device->vk_device, 1, &sync_primitives.in_flight), "Failed to reset vk swapchain fence");
}

void Swapchain::present() {
    auto current_index = frame_number % buffer_count;
    VkResult vk_result;
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &sync_data[current_index].render_completed;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &vk_swapchain;
    present_info.pImageIndices = &current_image_index;
    present_info.pResults = &vk_result;
    VK_CHECK_V(vkQueuePresentKHR(device->graphics_queue.vk_queue, &present_info), "Failed to present vk swapchain");

    ++frame_number;
}

Image& Swapchain::get_current_image() {
    return images[current_image_index];
}

ImageView& Swapchain::get_current_image_view() {
    return image_views[current_image_index];
}

FrameSyncData& Swapchain::get_current_sync() {
    return sync_data[frame_number % buffer_count];
}

void destroy_rhi(Rhi& rhi) {
    if (rhi.vk_instance) {
        vkDestroyInstance(rhi.vk_instance, nullptr);
        rhi.vk_instance = nullptr;
    }
}

Result Rhi::enumerate_adapters(uint32_t& adapter_count, AdapterInfo* out_adapters) const {
    if (!out_adapters) {
        VK_CHECK(vkEnumeratePhysicalDevices(vk_instance, &adapter_count, nullptr), "Failed to enumerate vk adapters");
        return Result::SUCCESS;
    } else {
        VkPhysicalDevice vk_adapters[MAX_VK_ADAPTERS];
        VK_CHECK(vkEnumeratePhysicalDevices(vk_instance, &adapter_count, vk_adapters),
                 "Failed to enumerate vk adapters");

        for (auto i = 0U; i < adapter_count; ++i) {
            VkPhysicalDeviceProperties2 vk_device_props{};
            VkPhysicalDeviceIDProperties vk_device_ids{};
            vk_device_ids.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
            vk_device_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            vk_device_props.pNext = &vk_device_ids;
            vkGetPhysicalDeviceProperties2(vk_adapters[i], &vk_device_props);

            out_adapters[i].device_id = vk_device_props.properties.deviceID;
            out_adapters[i].vendor_id = vk_device_props.properties.vendorID;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
            snprintf(out_adapters[i].device_name,
                     sizeof(out_adapters[i].device_name),
                     "%s",
                     vk_device_props.properties.deviceName);
#pragma GCC diagnostic pop
            out_adapters[i].type = map_vk_adapter_type(vk_device_props.properties.deviceType);
            memcpy(&out_adapters[i].device_luid, vk_device_ids.deviceLUID, sizeof(out_adapters[i].device_luid));
        }

        return Result::SUCCESS;
    }
}

Result Rhi::create_device(const AdapterInfo& adapter, Device& out_device) const {
    // Find selected adapter
    uint32_t adapter_count = MAX_VK_ADAPTERS;
    VkPhysicalDevice all_adapters[MAX_VK_ADAPTERS]{};
    VkPhysicalDevice vk_adapter = nullptr;
    VkPhysicalDeviceProperties2 device_props{};
    VkPhysicalDeviceIDProperties device_ids{};
    device_ids.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
    device_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    device_props.pNext = &device_ids;
    uint8_t device_id[8];
    memcpy(device_id, &adapter.device_luid, sizeof(adapter.device_luid));

    VK_CHECK(
        vkEnumeratePhysicalDevices(vk_instance, &adapter_count, all_adapters),
        "Failed to enumerate vk adapters on device creation");

    for (auto i = 0U; i < adapter_count; ++i) {
        vkGetPhysicalDeviceProperties2(all_adapters[i], &device_props);

        if (memcmp(device_ids.deviceLUID, device_id, sizeof(adapter.device_luid)) == 0) {
            vk_adapter = all_adapters[i];
        }
    }

    // Set up queue infos to create
    auto queue_indices = get_supported_queue_indices(vk_adapter);
    auto queue_prio = 1.0f;
    VkDeviceQueueCreateInfo queue_infos[3]{};

    queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_infos[0].queueCount = 1;
    queue_infos[0].pQueuePriorities = &queue_prio;
    queue_infos[0].queueFamilyIndex = queue_indices.graphics_index;
    queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_infos[1].queueCount = 1;
    queue_infos[1].pQueuePriorities = &queue_prio;
    queue_infos[1].queueFamilyIndex = queue_indices.compute_index;
    queue_infos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_infos[2].queueCount = 1;
    queue_infos[2].pQueuePriorities = &queue_prio;
    queue_infos[2].queueFamilyIndex = queue_indices.copy_index;

    // Create device
    const char* extension_names[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME };
    VkPhysicalDeviceSynchronization2Features sync2_feature{};
    sync2_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2_feature.synchronization2 = VK_TRUE;
    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature{};
    dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_feature.dynamicRendering = VK_TRUE;
    dynamic_rendering_feature.pNext = &sync2_feature;
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext = &dynamic_rendering_feature;
    create_info.pQueueCreateInfos = queue_infos;
    create_info.queueCreateInfoCount = 3;
    create_info.enabledExtensionCount = 2;
    create_info.ppEnabledExtensionNames = extension_names;
    create_info.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(vk_adapter, &create_info, nullptr, &out_device.vk_device), "Failed to create vk device");

    out_device.vk_adapter = vk_adapter;

    // Get created queues
    VkDeviceQueueInfo2 graphics_queue_info{};
    graphics_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    graphics_queue_info.queueFamilyIndex = queue_indices.graphics_index;
    graphics_queue_info.queueIndex = 0;
    vkGetDeviceQueue2(out_device.vk_device, &graphics_queue_info, &out_device.graphics_queue.vk_queue);
    out_device.graphics_queue.type = QueueType::GRAPHICS;
    out_device.graphics_queue.queue_family_index = queue_indices.graphics_index;

    VkDeviceQueueInfo2 compute_queue_info{};
    compute_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    compute_queue_info.queueFamilyIndex = queue_indices.compute_index;
    compute_queue_info.queueIndex = 0;
    vkGetDeviceQueue2(out_device.vk_device, &compute_queue_info, &out_device.compute_queue.vk_queue);
    out_device.compute_queue.type = QueueType::COMPUTE;
    out_device.compute_queue.queue_family_index = queue_indices.compute_index;

    VkDeviceQueueInfo2 copy_queue_info{};
    copy_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    copy_queue_info.queueFamilyIndex = queue_indices.copy_index;
    copy_queue_info.queueIndex = 0;
    vkGetDeviceQueue2(out_device.vk_device, &copy_queue_info, &out_device.copy_queue.vk_queue);
    out_device.copy_queue.type = QueueType::COPY;
    out_device.copy_queue.queue_family_index = queue_indices.copy_index;

    return Result::SUCCESS;
}

Result Rhi::create_swapchain_glfw(
    Device& device,
    GLFWwindow* window_handle,
    uint32_t width,
    uint32_t height,
    Format format,
    uint8_t image_count,
    Swapchain& out_swapchain) {
    if (glfwCreateWindowSurface(vk_instance, window_handle, nullptr, &out_swapchain.vk_surface)) {
        return Result::ERROR;
    }

    auto vk_format = map_vk_format(format);
    VkExtent2D vk_extent = { width, height };
    VkSwapchainCreateInfoKHR sc_create_info{};
    sc_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    sc_create_info.surface = out_swapchain.vk_surface;
    sc_create_info.minImageCount = image_count;
    sc_create_info.imageFormat = vk_format;
    sc_create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    sc_create_info.imageExtent = vk_extent;
    sc_create_info.imageArrayLayers = 1;
    sc_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    sc_create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    sc_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    sc_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sc_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    sc_create_info.clipped = VK_TRUE;
    sc_create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(
        vkCreateSwapchainKHR(device.vk_device, &sc_create_info, nullptr, &out_swapchain.vk_swapchain),
        "Failed to create vk swap chain");

    auto vk_image_count = 0U;
    VkImage vk_images[Swapchain::MAX_IMAGE_COUNT]{};
    VK_CHECK(
        vkGetSwapchainImagesKHR(device.vk_device, out_swapchain.vk_swapchain, &vk_image_count, nullptr),
        "Failed to retrieve swap chain image count");
    assert(vk_image_count == image_count);
    VK_CHECK(
        vkGetSwapchainImagesKHR(device.vk_device, out_swapchain.vk_swapchain, &vk_image_count, vk_images),
        "Failed to retrieve swap chain images");

    for (auto i = 0; i < image_count; ++i) {
        VkSemaphoreCreateInfo semaphore_create_info{};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fence_create_info{};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        out_swapchain.images[i].vk_image = vk_images[i];
        device.create_image_view(out_swapchain.images[i], format, out_swapchain.image_views[i]);

        VK_CHECK(
            vkCreateSemaphore(device.vk_device, &semaphore_create_info, nullptr, &out_swapchain.sync_data[i].
                image_available),
            "Failed to create vk semaphore for swap chain");
        VK_CHECK(
            vkCreateSemaphore(device.vk_device, &semaphore_create_info, nullptr, &out_swapchain.sync_data[i].
                render_completed),
            "Failed to create vk semaphore for swap chain");
        VK_CHECK(
            vkCreateFence(device.vk_device, &fence_create_info, nullptr, &out_swapchain.sync_data[i].in_flight),
            "Failed to create vk fence for swap chain");
        out_swapchain.sync_data[i].vk_device = device.vk_device;
    }

    out_swapchain.width = width;
    out_swapchain.height = height;
    out_swapchain.buffer_count = image_count;
    out_swapchain.device = &device;

    return Result::SUCCESS;
}

void Rhi::destroy_swapchain(Device& device, Swapchain& swapchain) {
    vkDestroySurfaceKHR(vk_instance, swapchain.vk_surface, nullptr);
    vkDestroySwapchainKHR(device.vk_device, swapchain.vk_swapchain, nullptr);
}
}

// #endif
