const std = @import("std");

const vk = @import("vulkan-zig");
const c = @import("glfw_c");
const Format = @import("constants.zig").Format;

const log = std.log.scoped(.vk);

pub const Rhi = struct {
    vk_instance: vk.Instance,
    vkw: vk.InstanceWrapper,

    const max_devices = 8;

    extern fn vkGetInstanceProcAddr(instance: vk.Instance, p_name: [*:0]const u8) callconv(.c) vk.PfnVoidFunction;
    extern fn vkGetDeviceProcAddr(device: vk.Device, p_name: [*:0]const u8) callconv(.c) vk.PfnVoidFunction;

    pub fn init() !Rhi {
        const validation_layers: [1][*:0]const u8 = .{"VK_LAYER_KHRONOS_validation"};
        const base: vk.BaseWrapper = vk.BaseWrapper.load(vkGetInstanceProcAddr);
        const app_info: vk.ApplicationInfo = .{
            .api_version = vk.API_VERSION_1_4.toU32(),
            .p_application_name = "fixme",
            .application_version = vk.makeApiVersion(0, 1, 0, 0).toU32(),
            .p_engine_name = "fixme",
            .engine_version = vk.makeApiVersion(0, 1, 0, 0).toU32(),
        };
        const create_info: vk.InstanceCreateInfo = .{
            .p_application_info = &app_info,
            .enabled_extension_count = 2,
            .pp_enabled_extension_names = &.{ vk.extensions.khr_surface.name, vk.extensions.khr_wayland_surface.name },
            .enabled_layer_count = 1,
            .pp_enabled_layer_names = &validation_layers,
        };
        const instance: vk.Instance = try base.createInstance(&create_info, null);
        const vki = vk.InstanceWrapper.load(instance, base.dispatch.vkGetInstanceProcAddr.?);
        return .{ .vk_instance = instance, .vkw = vki };
    }

    pub fn enumerateAdapters(self: *Rhi, adapters_out: []Adapter) !u32 {
        var adapters: [max_devices]vk.PhysicalDevice = undefined;
        var adapter_count: u32 = max_devices;
        _ = try self.vkw.enumeratePhysicalDevices(self.vk_instance, &adapter_count, &adapters);
        const convert_adapter_count = @min(adapter_count, adapters_out.len);
        for (adapters[0..convert_adapter_count], 0..) |physical_device, i| {
            var id_props = std.mem.zeroInit(vk.PhysicalDeviceIDProperties, .{});
            var adapter_props = std.mem.zeroInit(vk.PhysicalDeviceProperties2, .{ .p_next = &id_props });
            adapter_props.p_next = &id_props;
            self.vkw.getPhysicalDeviceProperties2(physical_device, &adapter_props);
            adapters_out[i] = .{
                .device_name = adapter_props.properties.device_name[0..128].*,
                .device_id = adapter_props.properties.device_id,
                .vendor_id = adapter_props.properties.vendor_id,
                .device_luid = @bitCast(id_props.device_luid),
                .type = .fromVk(adapter_props.properties.device_type),
                .handle = physical_device,
            };
        }
        return convert_adapter_count;
    }

    pub fn createDevice(self: *Rhi, adapter: *Adapter) !Device {
        const queue_families = self.findQueueFamilies(adapter);
        const queue_prio: [1]f32 = .{0.0};
        const queue_infos: [1]vk.DeviceQueueCreateInfo = .{
            .{
                .queue_count = 1,
                .queue_family_index = queue_families.graphics,
                .p_queue_priorities = &queue_prio,
            },
        };

        const create_info: vk.DeviceCreateInfo = .{
            .enabled_extension_count = 1,
            .pp_enabled_extension_names = &.{vk.extensions.khr_swapchain.name},
            .queue_create_info_count = 1,
            .p_queue_create_infos = &queue_infos,
            .enabled_layer_count = 0,
        };
        const vk_device = try self.vkw.createDevice(adapter.handle, &create_info, null);
        const vkd: vk.DeviceWrapper = vk.DeviceWrapper.load(vk_device, vkGetDeviceProcAddr);
        return .{ .vk_device = vk_device, .vkw = vkd, .queue_families = queue_families };
    }

    fn findQueueFamilies(self: *Rhi, adapter: *Adapter) QueueFamilyIndices {
        const max_families = 8;
        var families: [max_families]vk.QueueFamilyProperties2 = @splat(.{ .queue_family_properties = undefined });
        var family_count: u32 = 0;
        var indices: QueueFamilyIndices = .{};
        self.vkw.getPhysicalDeviceQueueFamilyProperties2(adapter.handle, &family_count, null);
        std.debug.assert(family_count <= max_families);
        self.vkw.getPhysicalDeviceQueueFamilyProperties2(adapter.handle, &family_count, &families);
        for (families[0..family_count], 0..) |family, i| {
            if (family.queue_family_properties.queue_flags.graphics_bit) {
                indices.graphics = @intCast(i);
            }
        }
        return indices;
    }
};

const QueueFamilyIndices = struct {
    graphics: u32 = std.math.maxInt(u32),
    compute: u32 = std.math.maxInt(u32),
    transfer: u32 = std.math.maxInt(u32),
};

pub const Adapter = struct {
    const Type = enum {
        discrete_gpu,
        integrated_gpu,
        cpu,
        other,

        pub fn fromVk(vk_type: vk.PhysicalDeviceType) Type {
            return switch (vk_type) {
                .discrete_gpu => .discrete_gpu,
                .integrated_gpu => .integrated_gpu,
                .cpu => .cpu,
                else => .other,
            };
        }
    };
    device_name: [128]u8,
    device_luid: u64,
    handle: vk.PhysicalDevice, // TODO: Remove and instead use a function to match the device luid (and maybe more) to find the right one
    device_id: u32,
    vendor_id: u32,
    type: Type,
};

pub const Device = struct {
    vk_device: vk.Device,
    vkw: vk.DeviceWrapper,
    queue_families: QueueFamilyIndices,

    pub fn createCommandList(self: *Device) !CommandList {
        const create_info: vk.CommandPoolCreateInfo = .{
            .queue_family_index = self.queue_families.graphics,
            .flags = .{ .reset_command_buffer_bit = true },
        };
        const cmd_pool = try self.vkw.createCommandPool(self.vk_device, &create_info, null);
        return .{ .vk_pool = cmd_pool };
    }
};

pub const CommandList = struct {
    vk_pool: vk.CommandPool,
};

pub const Swapchain = struct {
    const max_image_count = 4;

    vk_swapchain: vk.SwapchainKHR,
    vk_surface: vk.SurfaceKHR,
    device: *Device,
    rhi: *Rhi,
    images: [max_image_count]Image,
    image_views: [max_image_count]ImageView,
    render_done_sems: [max_image_count]Semaphore,
    current_image_index: u8 = 0,
    image_count: u8 = 0,
    width: u32 = 0,
    height: u32 = 0,
    format: Format,

    pub fn init(
        rhi: *Rhi,
        device: *Device,
        window_handle: *c.GLFWwindow,
        width: u32,
        height: u32,
        format: Format,
        image_count: u8,
    ) !Swapchain {
        std.debug.assert(image_count <= max_image_count);

        var raw_vk_surface: c.VkSurfaceKHR = undefined;
        _ = c.glfwCreateWindowSurface(@ptrFromInt(@intFromEnum(rhi.vk_instance)), window_handle, null, &raw_vk_surface);
        const vk_surface: vk.SurfaceKHR = @enumFromInt(@intFromPtr(raw_vk_surface));

        const create_info: vk.SwapchainCreateInfoKHR = .{
            .surface = vk_surface,
            .min_image_count = image_count,
            .image_extent = .{ .width = width, .height = height },
            .image_format = format.toVk(),
            .image_color_space = .srgb_nonlinear_khr,
            .image_array_layers = 1,
            .image_usage = .{ .color_attachment_bit = true, .transfer_dst_bit = true },
            .present_mode = .immediate_khr,
            .image_sharing_mode = .exclusive,
            .composite_alpha = .{ .opaque_bit_khr = true },
            .pre_transform = .{ .identity_bit_khr = true },
            .clipped = .true,
        };
        const vk_swapchain = try device.vkw.createSwapchainKHR(device.vk_device, &create_info, null);

        var sc_image_count: u32 = 0;
        _ = try device.vkw.getSwapchainImagesKHR(device.vk_device, vk_swapchain, &sc_image_count, null);
        std.debug.assert(sc_image_count == image_count);
        var vk_images: [max_image_count]vk.Image = undefined;
        _ = try device.vkw.getSwapchainImagesKHR(device.vk_device, vk_swapchain, &sc_image_count, &vk_images);
        var vk_image_views: [max_image_count]vk.ImageView = undefined;
        var vk_sems: [max_image_count]vk.Semaphore = undefined;
        var images: [max_image_count]Image = undefined;
        var image_views: [max_image_count]ImageView = undefined;
        var sems: [max_image_count]Semaphore = undefined;

        for (vk_images[0..image_count], 0..) |vk_image, i| {
            const sem_create_info: vk.SemaphoreCreateInfo = .{};
            vk_sems[i] = try device.vkw.createSemaphore(device.vk_device, &sem_create_info, null);
            sems[i] = .{ .vk_semaphore = vk_sems[i] };

            const view_create_info: vk.ImageViewCreateInfo = .{
                .format = format.toVk(),
                .image = vk_image,
                .view_type = vk.ImageViewType.@"2d",
                .components = .{
                    .r = vk.ComponentSwizzle.identity,
                    .g = vk.ComponentSwizzle.identity,
                    .b = vk.ComponentSwizzle.identity,
                    .a = vk.ComponentSwizzle.identity,
                },
                .subresource_range = .{
                    .aspect_mask = .{ .color_bit = true },
                    .base_mip_level = 0,
                    .level_count = 1,
                    .base_array_layer = 0,
                    .layer_count = 1,
                },
            };
            vk_image_views[i] = try device.vkw.createImageView(device.vk_device, &view_create_info, null);
            image_views[i] = .{ .vk_image_view = vk_image_views[i] };
            images[i] = .{ .vk_image = vk_image };
        }

        return .{
            .vk_swapchain = vk_swapchain,
            .vk_surface = vk_surface,
            .device = device,
            .rhi = rhi,
            .images = images,
            .image_views = image_views,
            .render_done_sems = sems,
            .image_count = image_count,
            .width = width,
            .height = height,
            .format = format,
        };
    }

    pub fn deinit(self: *Swapchain) void {
        self.rhi.vkw.destroySurfaceKHR(self.rhi.vk_instance, self.vk_surface, null);
        self.device.vkw.destroySwapchainKHR(self.device.vk_device, self.vk_swapchain, null);
        // TODO: Don't I need to destroy images, image views and semaphores?
    }
};

pub const Image = struct {
    vk_image: vk.Image,
};

pub const ImageView = struct {
    vk_image_view: vk.ImageView,
};

pub const Semaphore = struct {
    vk_semaphore: vk.Semaphore,
};

// TODO: Remove?
test "first" {
    var rhi: Rhi = try .init();
    var adapters: [Rhi.max_devices]Adapter = undefined;
    const adapter_count = try rhi.enumerateAdapters(&adapters);
    std.debug.assert(adapter_count > 0);
    const device = try rhi.createDevice(&adapters[0]);
    _ = device;
}
