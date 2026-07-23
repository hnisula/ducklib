const std = @import("std");

const Address = @import("Address.zig");
const Packet = @import("Packet.zig").Packet;
const shared = @import("shared.zig");
const Socket = @import("Socket.zig");

const log = std.log.scoped(.conn);

const Connection = @This();
addr: Address = .invalid,
last_tick: i64 = 0,
state: State = State.disconnected,
id: u32 = std.math.maxInt(u32),

var global_id: u32 = 0;

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

pub fn init(addr: Address) !Connection {
    const id = global_id;
    global_id += 1;
    log.info("[{d}] Connection to {f} initialized", .{ id, addr });
    return .{ .id = id, .state = State.disconnected, .last_tick = shared.nowNs() };
}

pub fn handle(self: *Connection, input: Input) ?Effect {
    const old_state = self.state;
    var new_state: ?State = null;
    var effect: ?Effect = null;

    switch (self.state) {
        .disconnected => {
            switch (input) {
                .connect => {
                    new_state = .connecting;
                    effect = .{ .send_packet = .{ .connect = .{} } };
                },
                else => {},
            }
        },
        .connecting => {
            switch (input) {
                .packet => |packet| {
                    switch (packet) {
                        .accept => {
                            new_state = .connected;
                        },
                        .reject => {
                            new_state = .disconnected;
                        },
                        else => unreachable,
                    }
                },
                else => {},
            }
        },
        else => {},
    }

    if (new_state) |s| {
        self.state = s;
    }
    log.debug("[{d}] Input {s} led to state change: {s} -> {s}", .{
        self.id,
        @tagName(input),
        @tagName(old_state),
        @tagName(new_state.?),
    });
    return effect;
}
