const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // net
    const net_module = b.createModule(.{ .root_source_file = b.path("src/net/net.zig"), .target = target, .optimize = optimize });
    const net_lib = b.addLibrary(.{
        .name = "net",
        .linkage = .static,
        .root_module = net_module,
    });

    // net-conn-test
    const net_conn_test = b.addExecutable(.{ .name = "net-conn-test", .root_module = b.createModule(.{
        .root_source_file = b.path("tests/net/conn_test.zig"),
        .target = target,
        .optimize = optimize,
    }) });
    net_conn_test.root_module.addImport("net", net_module);
    const net_conn_test_install = b.addInstallArtifact(net_conn_test, .{});
    const net_conn_test_step = b.step("net-conn-test", "Build net-conn-test program");
    net_conn_test_step.dependOn(&net_conn_test_install.step);

    // net tests
    const net_tests = b.addTest(.{ .root_module = net_module });

    // install build
    b.installArtifact(net_lib);
    b.getInstallStep().dependOn(&net_conn_test_install.step);

    // test
    const test_step = b.step("test", "Run tests");
    test_step.dependOn(&b.addRunArtifact(net_tests).step);
}
