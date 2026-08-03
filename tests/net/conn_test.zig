const std = @import("std");
const net = @import("net");
const expect = std.testing.expect;

pub fn main(init: std.process.Init) !void {
    const t1 = try std.Thread.spawn(.{}, serverWorker, .{init.io});
    const t2 = try std.Thread.spawn(.{}, clientWorker, .{init.io});

    t2.join();
    t1.join();
}

fn clientWorker(io: std.Io) !void {
    _ = io;

    const to = net.Address.initIp4(.{ 127, 0, 0, 1 }, 12800);
    var client = try net.Client.init(12801);
    defer client.deinit();

    try client.connect(to);

    while (true) {
        try client.tick();
    }
    // TODO: Send some stuff
    // TODO: Receive some stuff?
    // TODO: Check received data
    // TODO: Disconnect
}

fn serverWorker(io: std.Io) !void {
    _ = io;

    var server = try net.Server.init(12800);
    defer server.deinit();
    
    while (true) {
        try server.tick();
    }
}