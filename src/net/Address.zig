const std = @import("std");
const linux = std.os.linux;
const posix = std.posix;
const Address = @This();

sa: linux.sockaddr.in6,

pub const sa_len: u32 = @sizeOf(linux.sockaddr.in6);

pub fn initIp4(octets: [4]u8, port: u16) Address {
    const addr: Address = .{ .sa = .{
        .family = posix.AF.INET6,
        .port = std.mem.nativeToBig(u16, port),
        .addr = .{
            0,         0,         0,         0,
            0,         0,         0,         0,
            0,         0,         0xff,      0xff,
            octets[0], octets[1], octets[2], octets[3],
        },
        .flowinfo = 0,
        .scope_id = 0,
    } };

    return addr;
}

pub fn initIp6(octets: [16]u8, port: u16) Address {
    const addr: Address = .{ .sa = .{
        .family = posix.AF.INET6,
        .port = std.mem.nativeToBig(u16, port),
        .addr = octets,
        .flowinfo = 0,
        .scope_id = 0,
    } };
    return addr;
}

pub fn format(self: *const Address, w: *std.Io.Writer) !void {
    if (self.isIp4()) {
        const sa_ip4: *const linux.sockaddr.in = @ptrCast(&self.sa);
        const bytes: [4]u8 = @bitCast(sa_ip4.addr);
        try w.print("{}.{}.{}.{}:{}", .{ bytes[0], bytes[1], bytes[2], bytes[3], std.mem.bigToNative(u16, sa_ip4.port) });
    } else {
        const sa_ip6: *const linux.sockaddr.in6 = @ptrCast(&self.sa);
        try w.print("[", .{});
        for (0..8) |i| {
            const n = std.mem.readInt(u16, sa_ip6.addr[i * 2 ..][0..2], .big);
            if (i != 0) try w.writeByte(':');
            try w.print("{x}", .{n});
        }
        try w.print("]:{}", .{std.mem.bigToNative(u16, sa_ip6.port)});
    }
}

pub fn isIp4(self: *const Address) bool {
    const ip4_prefix = [_]u8{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };
    return std.mem.eql(u8, self.sa.addr[0..12], &ip4_prefix);
}
