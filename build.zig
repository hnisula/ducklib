const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const net = b.addLibrary(.{
        .name = "net",
        .linkage = .static,
        .root_module = b.createModule(.{
            .root_source_file = b.path("src/net/Socket.zig"),
            .target = target,
            .optimize = optimize,
        }),
    });

    b.installArtifact(net);

    const test_step = b.step("test", "Run unit tests");

    const unit_tests = b.addTest(.{ .root_module = b.createModule(.{
        .root_source_file = b.path("tests/net/main.zig"),
        .target = target,
    }) });
    const socket_tests = b.addTest(.{ .root_module = b.createModule(.{
        .root_source_file = b.path("src/net/Socket.zig"),
        .target = target,
    }) });

    const run_unit_tests = b.addRunArtifact(unit_tests);
    const run_socket_tests = b.addRunArtifact(socket_tests);
    test_step.dependOn(&run_unit_tests.step);
    test_step.dependOn(&run_socket_tests.step);
}
