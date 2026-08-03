const std = @import("std");

const Address = @import("Address.zig");
const Connection = @import("Connection.zig");
const Packet = @import("Packet.zig").Packet;
const shared = @import("shared.zig");
const Socket = @import("Socket.zig");

const log = std.log.scoped(.client);

const Client = @This();
socket: Socket,
conn: Connection,

pub fn init(port: u16) !Client {
    log.info("Client initialized on port {d}", .{port});
    return .{ .socket = try Socket.open(port), .conn = .{} };
}

pub fn deinit(self: *Client) void {
    self.socket.close();
    log.info("Client shut down", .{});
}

pub fn connect(self: *Client, to: Address) !void {
    var buffer: [Socket.MTU]u8 = undefined;
    self.conn = try .init(to);
    log.info("Client using connection {d}", .{self.conn.id});
    const effect = self.conn.handle(.{ .connect = .{ .addr = to } });
    if (effect == null) {
        return;
    }
    const size = try effect.?.send_packet.write(&buffer);
    _ = try self.socket.sendTo(buffer[0..size], to);
}

pub fn tick(self: *Client) !void {
    var buffer: [Socket.MTU]u8 = undefined;

    while (try self.socket.receive(&buffer)) |received| {
        const response = try Packet.read(received.packet);
        const effect = self.conn.handle(.{ .packet = response });
        _ = effect;
    }
}
