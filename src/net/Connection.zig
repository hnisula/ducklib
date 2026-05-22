const std = @import("std");
const linux = std.os.linux;
const Connection = @This();
const Socket = @import("Socket.zig");
const Address = @import("Address.zig");

socket: Socket,
address: Address,
state: State = State.DISCONNECTED,

const State = enum { CONNECTING, CONNECTED, DISCONNECTED };

pub fn connect(socket: Socket, address: Address) !Connection {
    return .{ .socket = socket, .address = address, .state = State.CONNECTED };
}

pub fn close(self: *Connection) void {
    _ = self;
}
