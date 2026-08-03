const std = @import("std");

const c = @import("glfw_c");

const Window = @This();

const log = std.log.scoped(.window);

handle: ?*c.GLFWwindow,

var is_init = false;

const WindowError = error{
    FailedInit,
};

pub fn init(width: u32, height: u32, title: []const u8) !Window {
    if (!is_init) {
        _ = c.glfwSetErrorCallback(errorCallback);
        _ = c.glfwInit();
        is_init = true;
    }
    _ = c.glfwWindowHint(c.GLFW_CLIENT_API, c.GLFW_NO_API);
    const handle = c.glfwCreateWindow(@intCast(width), @intCast(height), title.ptr, null, null) orelse return WindowError.FailedInit;
    return .{ .handle = handle };
}

pub fn deinit(self: *Window) void {
    if (self.handle) |handle| {
        c.glfwDestroyWindow(handle);
    }
    if (is_init) {
        c.glfwTerminate();
    }
}

pub fn close(self: *Window) void {
    c.glfwSetWindowShouldClose(self.handle.?, 1);
}

pub fn processMessages(self: *Window) void {
    _ = self;
    c.glfwPollEvents();
}

fn errorCallback(err: c_int, desc: [*c]const u8) callconv(.c) void {
    log.err("GLFW: {d} - {s}", .{ err, desc });
}
