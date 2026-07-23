const std = @import("std");

pub fn build(b: *std.Build) void {
    const t = b.standardTargetOptions(.{
        .default_target = .{
            .glibc_version = .{ .major = 2, .minor = 38, .patch = 0 }, // NOTE: Linux fix for vk needing glibc and 0.16.0 compatibility with it
        },
    });
    const o = b.standardOptimizeOption(.{});

    // Platform
    const platform_module = b.createModule(.{ .root_source_file = b.path("src/platform/platform.zig"), .target = t, .optimize = o });
    platform_module.linkSystemLibrary("glfw", .{});
    const glfw = b.addTranslateC(.{
        .root_source_file = .{ .cwd_relative = "src/platform/c/glfw.h" },
        .target = t,
        .optimize = o,
        .link_libc = true,
    });
    glfw.addIncludePath(.{ .cwd_relative = "/usr/include" });
    glfw.defineCMacro("GLFW_INCLUDE_VULKAN", null);
    const glfw_module = glfw.createModule();
    platform_module.addImport("glfw_c", glfw_module);

    // Net
    const net_module = b.createModule(.{ .root_source_file = b.path("src/net/net.zig"), .target = t, .optimize = o });

    // Render
    // NOTE: libc dependency here for vk
    const render_module = b.createModule(.{ .root_source_file = b.path("src/render/render.zig"), .target = t, .optimize = o, .link_libc = true });
    const vk_headers = b.dependency("vulkan_headers", .{});
    const vk_zig_module = b.dependency("vulkan", .{ .registry = vk_headers.path("registry/vk.xml") }).module("vulkan-zig");
    render_module.addLibraryPath(.{ .cwd_relative = "/usr/lib" }); // NOTE: Linux fix for vk and glibc with zig 0.16.0
    render_module.linkSystemLibrary("vulkan", .{ .preferred_link_mode = .dynamic });
    render_module.addImport("vulkan-zig", vk_zig_module);
    render_module.addImport("glfw_c", glfw_module);

    // Net: conn-test
    const net_conn_test = b.addExecutable(.{
        .name = "net-conn-test",
        .root_module = b.createModule(.{ .root_source_file = b.path("tests/net/conn_test.zig"), .target = t, .optimize = o }),
    });
    net_conn_test.root_module.addImport("net", net_module);
    b.installArtifact(net_conn_test);

    // Render: hello-triangle
    const render_hello_triangle = b.addExecutable(.{
        .name = "render-hello-triangle",
        .root_module = b.createModule(.{ .root_source_file = b.path("tests/render/hello_triangle.zig"), .target = t, .optimize = o }),
    });
    render_hello_triangle.root_module.addImport("render", render_module);
    render_hello_triangle.root_module.addImport("platform", platform_module);
    b.installArtifact(render_hello_triangle);

    // install step
    const install = b.getInstallStep();
    install.dependOn(&net_conn_test.step);
    install.dependOn(&render_hello_triangle.step);

    // tests step
    const net_tests = b.addTest(.{ .root_module = net_module });
    const render_tests = b.addTest(.{ .root_module = render_module });
    const @"test" = b.step("test", "Run tests");
    @"test".dependOn(&net_tests.step);
    @"test".dependOn(&render_tests.step);
    const install_test = b.step("install_test", "Output test files to enable IDE debugging of tests");
    const install_net_tests = b.addInstallArtifact(net_tests, .{ .dest_dir = .{ .override = .{ .custom = "tests" } } });
    const install_render_tests = b.addInstallArtifact(render_tests, .{ .dest_dir = .{ .override = .{ .custom = "tests" } } });
    install_test.dependOn(&install_net_tests.step);
    install_test.dependOn(&install_render_tests.step);

    // check step
    const check = b.step("check", "For ZLS's build-on-save");
    check.dependOn(&b.addTest(.{ .root_module = net_module }).step);
    check.dependOn(&b.addTest(.{ .root_module = render_module }).step);
}
