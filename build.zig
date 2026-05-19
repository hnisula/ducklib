const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // net
    const net_module = b.createModule(.{
        .root_source_file = b.path("src/net/net.zig"),
        .target = target,
        .optimize = optimize
    });
    const net_lib = b.addLibrary(.{
        .name = "net",
        .linkage = .static,
        .root_module = net_module,
    });
    b.installArtifact(net_lib);
    
    // connection-test
    const conn_test = b.addExecutable(.{
        .name = "conn-test",
        .root_module = b.createModule(.{
            .root_source_file = b.path("tests/net/conn_test.zig"),
            .target = target,
            .optimize = optimize,
        })
    });
    conn_test.root_module.addImport("net", net_module);
    const conn_test_install = b.addInstallArtifact(conn_test, .{});
    const conn_test_step = b.step("conn-test", "Compile connection test program");
    conn_test_step.dependOn(&conn_test_install.step);

    // TEST tests
    // const test_step = b.step("test", "Run unit tests");
    // const unit_tests = b.addTest(.{ .root_module = b.createModule(.{
    //     .root_source_file = b.path("tests/net/conn_test.zig"),
    //     .target = target,
    // }) });
    // const socket_tests = b.addTest(.{ .root_module = b.createModule(.{
    //     .root_source_file = b.path("src/net/Socket.zig"),
    //     .target = target,
    // }) });
    // const run_unit_tests = b.addRunArtifact(unit_tests);
    // const run_socket_tests = b.addRunArtifact(socket_tests);
    // test_step.dependOn(&run_unit_tests.step);
    // test_step.dependOn(&run_socket_tests.step);

    // TEST debugging tests
    // const test_artifact = b.addInstallArtifact(socket_tests, .{ .dest_dir = .{ .override = .{ .custom = "bin" } } });
    // const install_test_step = b.step("install_test", "Create test binaries for debugging");
    // install_test_step.dependOn(&test_artifact.step);
}
