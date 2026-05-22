const std = @import("std");
const posix = std.posix;
const linux = std.os.linux;
const expectEqual = std.testing.expectEqual;
const Socket = @This();
const Address = @import("Address.zig");

fd: linux.fd_t,

const PacketHeader = struct {
    seq: u32 = 0,
    ack_seq: u32 = 0,
    ack_bits: u32 = 0,
    size: u16 = 0,

    pub fn write(buffer: []u8) !void {
        _ = buffer;
    }
};

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
    const sent_bytes = linux.sendto(self.fd, data.ptr, data.len, 0, @ptrCast(&to.sa), Address.sa_len);
    switch (std.posix.errno(sent_bytes)) {
        .SUCCESS => return sent_bytes,
        else => return SendError.Unknown,
    }
}

const ReceiveError = error{ InvalidArgument, Unknown };

pub fn receive(self: *Socket, buffer: []u8, from: *Address) ReceiveError!usize {
    var sa_len: linux.socklen_t = Address.sa_len;
    const received_bytes = linux.recvfrom(self.fd, buffer.ptr, buffer.len, 0, @ptrCast(&from.sa), &sa_len);
    std.debug.assert(sa_len == @sizeOf(linux.sockaddr.in6));
    switch (std.posix.errno(received_bytes)) {
        .SUCCESS => return received_bytes,
        .AGAIN => return 0,
        .INVAL => return ReceiveError.InvalidArgument,
        else => |err| { // TODO: Map errors and just return
            std.log.err("Failed to receive socket bytes ({s})", .{@tagName(err)});
            return ReceiveError.Unknown;
        },
    }
}

test "open socket" {
    const addr = Address.initIp4(.{ 127, 0, 0, 1 }, 12601);
    var sender = try Socket.open(12600);
    defer sender.close();
    var receiver = try Socket.open(12601);
    defer receiver.close();
    var buffer: [512]u8 = @splat(0);

    const sent = try sender.sendTo(&.{ 1, 2, 3, 4 }, addr);
    try expectEqual(4, sent);
    const received = try receiver.receive(&buffer);
    try expectEqual(4, received);
    return;
}
