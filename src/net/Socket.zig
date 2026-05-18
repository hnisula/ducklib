const std = @import("std");
const posix = std.posix;
const linux = std.os.linux;
const Socket = @This();
const Address = @import("Address.zig");

fd: linux.fd_t,

const OpenError = error{ AddressInUse, Unknown };

pub fn open(port: u16) OpenError!Socket {
    const socket_rc = linux.socket(linux.AF.INET6, linux.SOCK.DGRAM | linux.SOCK.NONBLOCK | linux.SOCK.CLOEXEC, linux.IPPROTO.UDP);
    switch (posix.errno(socket_rc)) {
        .SUCCESS => {},
        else => |err| {
            std.log.err("Failed to create socket: {s}", .{@tagName(err)});
            return OpenError.Unknown;
        },
    }
    const fd: linux.fd_t = @intCast(socket_rc);
    errdefer _ = linux.close(fd);

    const sock_addr: linux.sockaddr.in6 = .{
        .family = linux.AF.INET6,
        .port = std.mem.nativeToBig(u16, port),
        .addr = .{0} ** 16,
        .flowinfo = 0,
        .scope_id = 0,
    };
    const bind_rc = linux.bind(fd, @ptrCast(&sock_addr), @sizeOf(@TypeOf(sock_addr)));
    switch (posix.errno(bind_rc)) {
        .SUCCESS => {},
        .ADDRINUSE => return OpenError.AddressInUse,
        else => return OpenError.Unknown,
    }

    return Socket{ .fd = fd };
}

pub fn close(self: *Socket) void {
    _ = linux.close(self.fd);
}

const SendError = error{Unknown};

pub fn sendTo(self: *Socket, data: []const u8, to: Address) SendError!usize {
    const sock_addr, const sock_addr_len = sockAddr(to);
    const sent_bytes = linux.sendto(self.fd, data.ptr, data.len, 0, sock_addr, sock_addr_len);
    switch (std.posix.errno(sent_bytes)) {
        .SUCCESS => return sent_bytes,
        else => return SendError.Unknown,
    }
}

pub fn receive(self: *Socket, buffer: []u8) !usize {
    var addr: linux.sockaddr.storage = undefined; // TODO: Fix
    var addr_len: linux.socklen_t = 0;
    const received_bytes = linux.recvfrom(self.fd, buffer.ptr, buffer.len, 0, &addr, &addr_len);
    return received_bytes;
}

fn sockAddr(addr: Address) struct { ptr: *const linux.sockaddr, len: linux.socklen_t } {
    return switch (addr.sa) {
        .ip4 => return .{ addr.ip4, @sizeOf(linux.sockaddr.in) },
        .ip6 => return .{ addr.ip6, @sizeOf(linux.sockaddr.in6) },
    };
}

test "open socket" {
    const addr = Address.ip4(.{ 127, 0, 0, 1 }, 12601);
    var sender = try Socket.open(12600);
    defer sender.close();
    var receiver = try Socket.open(12601);
    defer receiver.close();
    const buffer: [512]u8 = @splat(0);

    try sender.sendTo(&.{ 1, 2, 3, 4 }, addr);
    try receiver.receive(buffer);
    return;
}
