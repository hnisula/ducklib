const std = @import("std");

const Address = @import("Address.zig");
const Packet = @import("Packet.zig").Packet;
const shared = @import("shared.zig");
const Socket = @import("Socket.zig");

const Connection = @This();
state: State = State.disconnected,
last_tick: i64 = 0,

const State = enum { connecting, connected, disconnected };

pub const Input = union(enum) {
    connect: struct {
        addr: Address,
    },
    disconnect,
    packet: Packet,
};

// TODO: Reconsider the name
pub const Effect = union(enum) {
    send_packet: Packet,
};

pub fn init() !Connection {
    return .{ .state = State.disconnected, .last_tick = shared.nowNs() };
}

pub fn handle(self: *Connection, input: Input) ?Effect {
    switch (self.state) {
        .disconnected => {
            switch (input) {
                .connect => {
                    self.state = .connecting;
                    return .{ .send_packet = .{ .connect = .{} } };
                },
                else => return null,
            }
        },
        .connecting => {
            switch (input) {
                .packet => |packet| {
                    switch (packet) {
                        .accept => {
                            self.state = .connected;
                            std.debug.print("Connection accepted\n", .{});
                        },
                        .reject => |reject| {
                            self.state = .disconnected;
                            std.debug.print("Connection rejected: {s}\n", .{reject.reason});
                        },
                        else => unreachable,
                    }
                },
                else => {},
            }
        },
        else => {
            std.debug.print("Unhandled connection state input\n", .{});
        },
    }
    return null;
}
