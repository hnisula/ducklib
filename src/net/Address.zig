const std = @import("std");
const linux = std.os.linux;
const posix = std.posix;
const Address = @This();

sa: linux.sockaddr.storage,
sa_len: u8,

pub fn ip4(octets: [4]u8, port: u16) Address {
    var addr: Address = .{ .sa = std.mem.zeroes(posix.sockaddr.storage), .sa_len = @sizeOf(posix.sockaddr.in) };
    const sockaddr_ip4: *posix.sockaddr.in = @ptrCast(&addr.sa);
    sockaddr_ip4.* = .{
        .family = posix.AF.INET,
        .port = std.mem.nativeToBig(u16, port),
        .addr = @bitCast(octets),
        .zero = .{0} ** 8,
    };

    return addr;
}
