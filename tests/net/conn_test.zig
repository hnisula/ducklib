const std = @import("std");
const net = @import("net");
const expect = std.testing.expect;

pub fn main() !void {
    const addr2 = net.Address.ip4(.{ 127, 0, 0, 1 }, 12800);
    var conn1 = try net.Connection.connect(addr2);
    defer conn1.close();
}
