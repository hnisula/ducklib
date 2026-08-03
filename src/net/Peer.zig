const std = @import("std");

const Socket = @import("Socket.zig");
const Connection = @import("Connection.zig");
const Address = @import("Address.zig");
const Packet = @import("Packet.zig");

const Peer = @This();

socket: Socket,
conn: Connection,

pub fn init(port: u16) !Peer {
    return .{ .socket = try Socket.open(port), .conn = try Connection.init() };
}

pub fn deinit(self: *Peer) void {
    self.socket.close();
}

pub fn connect(self: *Peer, to: Address) !void {
    var buffer: [Socket.MTU]u8 = undefined;
    const effect = self.conn.handle(.{ .connect = .{ .addr = to } });
    if (effect == null) {
        return;
    }
    try effect.?.send_packet.write(&buffer);
    _ = try self.socket.sendTo(&buffer, to);
}

pub fn tick(self: *Peer) !void {
    var buffer: [Socket.MTU]u8 = undefined;

    while (try self.socket.receive(&buffer)) |received| {
        const response = try Packet.read(received.packet);
        const effect = self.conn.handle(.{ .packet = response });
        _ = effect;
    }
}
