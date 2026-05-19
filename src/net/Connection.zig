const std = @import("std");
const linux = std.os.linux;
const Connection = @This();
const Address = @import("Address.zig");

address: Address,

pub fn connect(address: Address) !Connection {
    return .{ .address = address };
}

pub fn close(self: *Connection) void {
    _ = self;
}