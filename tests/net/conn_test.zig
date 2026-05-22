const std = @import("std");
const net = @import("net");
const expect = std.testing.expect;

pub fn main(init: std.process.Init) !void {
    const t1 = try std.Thread.spawn(.{}, serverSock, .{init.io});
    const t2 = try std.Thread.spawn(.{}, clientSock, .{init.io});

    t2.join();
    t1.join();
}

fn clientSock(io: std.Io) !void {
    const to = net.Address.initIp4(.{ 127, 0, 0, 1 }, 12800);
    var socket = try net.Socket.open(12801);
    var buffer: [1024]u8 = undefined;
    var i: u32 = 0;

    try io.sleep(.{ .nanoseconds = 500_000_000 }, .awake);

    while (true) {
        @memset(&buffer, @intCast(i));
        _ = try socket.sendTo(&buffer, to);

        i += 1;

        try io.sleep(.{ .nanoseconds = 200_000_000 }, .awake);

        if (i == 24) {
            break;
        }
    }

    std.debug.print("Client done\n", .{});
}

fn serverSock(io: std.Io) !void {
    // const from = net.Address.ip4(.{ 127, 0, 0, 1 }, 12801);
    var socket = try net.Socket.open(12800);
    var buffer: [1024]u8 = undefined;
    var i: u32 = 0;
    var addr: net.Address = undefined;

    while (true) {
        const bytes_received = try socket.receive(&buffer, &addr);
        if (bytes_received > 0) {
            std.debug.print("[{d}]: Received bytes: {d} from {f}\n", .{ i, bytes_received, addr });
            i += 1;
        }

        if (i == 24) {
            break;
        }

        try io.sleep(.{ .nanoseconds = 100_000_000 }, .awake);
    }

    std.debug.print("Server done\n", .{});
}

// fn client() !void {
//     const addr = net.Address.ip4(.{ 127, 0, 0, 1 }, 12800);
//     const socket = net.Socket.open(12801);
//     var conn = try net.Connection.connect(addr);
//     defer conn.close();
//
//     // TODO: Connection request
//     // TODO: Send some stuff
//     // TODO: Receive some stuff?
//     // TODO: Check received data
//     // TODO: Disconnect
// }
//
// fn server() !void {
//     const addr = net.Address.ip4(.{ 127, 0, 0, 1 }, 12801);
//     // var listener
// }
