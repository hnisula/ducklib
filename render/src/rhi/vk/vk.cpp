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

struct QueueFamily {
    uint32_t graphics_index = -1;
    uint32_t compute_index = -1;
    uint32_t copy_index = -1;
};

QueueFamily get_supported_queue_indices(VkPhysicalDevice vk_device) {
    VkQueueFamilyProperties2 family_prop_sets[MAX_QUEUE_FAMILIES]{};
    uint32_t family_count = 0;

    for (auto& family_props : family_prop_sets) {
        family_props.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    }

    vkGetPhysicalDeviceQueueFamilyProperties2(vk_device, &family_count, nullptr);
    assert(family_count <= MAX_QUEUE_FAMILIES);
    vkGetPhysicalDeviceQueueFamilyProperties2(vk_device, &family_count, family_prop_sets);

    QueueFamily family_indices{};

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
    const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
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
        VK_CHECK(vkEnumeratePhysicalDevices(vk_instance, &adapter_count, vk_adapters), "Failed to enumerate vk adapters");

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
            snprintf(out_adapters[i].device_name, sizeof(out_adapters[i].device_name), "%s", vk_device_props.properties.deviceName);
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
    auto extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_infos;
    create_info.queueCreateInfoCount = 3;
    create_info.enabledExtensionCount = 1;
    create_info.ppEnabledExtensionNames = &extension_names;
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

    VkDeviceQueueInfo2 compute_queue_info{};
    compute_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    compute_queue_info.queueFamilyIndex = queue_indices.compute_index;
    compute_queue_info.queueIndex = 0;
    vkGetDeviceQueue2(out_device.vk_device, &compute_queue_info, &out_device.compute_queue.vk_queue);
    out_device.compute_queue.type = QueueType::COMPUTE;

    VkDeviceQueueInfo2 copy_queue_info{};
    copy_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    copy_queue_info.queueFamilyIndex = queue_indices.copy_index;
    copy_queue_info.queueIndex = 0;
    vkGetDeviceQueue2(out_device.vk_device, &copy_queue_info, &out_device.copy_queue.vk_queue);
    out_device.copy_queue.type = QueueType::COPY;

    return Result::SUCCESS;
}

Result Rhi::create_swapchain_glfw(
    Device& device,
    GLFWwindow* window_handle,
    uint32_t width,
    uint32_t height,
    Format format,
    Swapchain& out_swapchain) {
    if (glfwCreateWindowSurface(vk_instance, window_handle, nullptr, &out_swapchain.vk_surface)) {
        return Result::ERROR;
    }

    auto vk_format = map_vk_format(format);
    VkExtent2D vk_extent = {width, height};
    VkSwapchainCreateInfoKHR sc_create_info{};
    sc_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    sc_create_info.surface = out_swapchain.vk_surface;
    sc_create_info.minImageCount = 2;
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

    auto image_count = 0U;
    VK_CHECK(
        vkGetSwapchainImagesKHR(device.vk_device, out_swapchain.vk_swapchain, &image_count, nullptr),
        "Failed to retrieve swap chain image count");
    assert(image_count <= Swapchain::MAX_IMAGE_COUNT);
    VK_CHECK(
        vkGetSwapchainImagesKHR(device.vk_device, out_swapchain.vk_swapchain, &image_count, out_swapchain.buffers),
        "Failed to retrieve swap chain images");
    
    out_swapchain.width = width;
    out_swapchain.height = height;
    out_swapchain.buffer_count = image_count;

    return Result::SUCCESS;
}
}

// #endif
