const std = @import("std");

const render = @import("render");
const platform = @import("platform");

pub fn main() !void {
    var rhi: render.rhi.Rhi = try .init();
    var adapters: [8]render.rhi.Adapter = undefined;
    _ = try rhi.enumerateAdapters(&adapters);
    var window: platform.Window = try .init(600, 400, "hello");
    defer window.deinit();
    var device = try rhi.createDevice(&adapters[0]);
    const swapchain = try render.rhi.Swapchain.init(&rhi, &device, window.handle.?, 600, 400, render.Format.r8g8b8a8_unorm, 2);
    _ = swapchain;
    const cmd_list = try device.createCommandList();
    _ = cmd_list;

    while (true) {
        window.processMessages();

        // TODO: Clear RT
        // TODO: Present
        // TODO: Acquire next swapchain image and repeat
    }
}
