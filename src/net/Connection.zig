const std = @import("std");
const linux = std.os.linux;

pub const Connection = struct {
    address: []const u8 = undefined,

    pub fn open(address: []u8) !Connection {
        return .{ .address = address };
    }
};
