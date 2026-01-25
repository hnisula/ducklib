// #ifdef DL_VK

#include <stdexcept>
#include <GLFW/glfw3.h>
#include <cstring>

#include "vk_util.h"
#include "ducklib/render/rhi/vk.h"

namespace ducklib::render {
constexpr auto MAX_QUEUE_FAMILIES = 32U;

void Device::create_queue(QueueType type, CommandQueue& out_queue) {
    uint32_t family_count = 0;
    uint32_t graphics_family_index = -1;
    uint32_t copy_family_index = -1;
    uint32_t compute_family_index = -1;
    VkQueueFamilyProperties2 family_props[MAX_QUEUE_FAMILIES]{};

    for (auto & family_prop : family_props) {
        family_prop.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    }

    vkGetPhysicalDeviceQueueFamilyProperties2(vk_adapter, &family_count, nullptr);
    vkGetPhysicalDeviceQueueFamilyProperties2(vk_adapter, &family_count, family_props);
    
    // Find which queue families to choose for the different queues.
    for (auto i = 0U; i < family_count; ++i) {
        auto graphics = family_props[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        auto compute = family_props[i].queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT;
        auto copy = family_props[i].queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT;
        
        if (graphics && compute && copy) {
            graphics_family_index = i;
        }
        
        if (!graphics && compute) {
            compute_family_index = i;
        }
        
        if (!graphics && !compute && copy) {
            copy_family_index = i;
        }
    }
    
    uint32_t family = -1;
    
    switch (type) {
    case QueueType::GRAPHICS: family = graphics_family_index; break;
    case QueueType::COMPUTE: family = compute_family_index; break;
    case QueueType::COPY: family = copy_family_index; break;
    }
    
    auto queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    queue_create_info.queueFamilyIndex = family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    THIS MUST BE DONE WHEN CREATING DEVICE INSTEAD. Must be updated for d3d12, too
    
    out_queue.type = type;
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
            snprintf(out_adapters[i].device_name, sizeof(out_adapters[i].device_name), "%s", vk_device_props.properties.deviceName);
            out_adapters[i].type = map_vk_adapter_type(vk_device_props.properties.deviceType);
            memcpy(&out_adapters[i].device_luid, vk_device_ids.deviceLUID, sizeof(out_adapters[i].device_luid));
        }

        return Result::SUCCESS;
    }
}

Result Rhi::create_device(const AdapterInfo& adapter, Device& out_device) const {
    // Choose adapter
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

    // Create device
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    VK_CHECK(vkCreateDevice(vk_adapter, &create_info, nullptr, &out_device.vk_device), "Failed to create vk device");
    out_device.vk_adapter = vk_adapter;

    out_device.create_queue(QueueType::GRAPHICS, out_device.graphics_queue);
    out_device.create_queue(QueueType::COPY, out_device.graphics_queue);

    return Result::SUCCESS;
}
}

// #endif
