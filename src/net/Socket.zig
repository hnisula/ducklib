const std = @import("std");
const posix = std.posix;
const linux = std.os.linux;
const expectEqual = std.testing.expectEqual;
const expectError = std.testing.expectError;

const Address = @import("Address.zig");

const log = std.log.scoped(.socket);

const Socket = @This();
fd: linux.fd_t,
rng: std.Random.DefaultPrng,
loss_rate: f32 = 0.0,
port: u16 = 0,

pub const MTU: u32 = 1200;

const PacketHeader = struct {
    seq: u32 = 0,
    ack_seq: u32 = 0,
    ack_bits: u32 = 0,
    size: u16 = 0,

    pub fn write(buffer: []u8) !void {
        _ = buffer;
    }
};

const OpenError = error{ AddressInUse, Unknown, PermissionDenied };

pub fn open(port: u16) OpenError!Socket {
    const socket_rc = linux.socket(linux.AF.INET6, linux.SOCK.DGRAM | linux.SOCK.NONBLOCK | linux.SOCK.CLOEXEC, linux.IPPROTO.UDP);
    switch (posix.errno(socket_rc)) {
        .SUCCESS => {},
        .ACCES => return error.PermissionDenied,
        else => |err| {
            log.err("[:{d}] Failed to create socket: {s}", .{ port, @tagName(err) });
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

    var seed: u64 = undefined;
    _ = linux.getrandom(std.mem.asBytes(&seed), @sizeOf(@TypeOf(seed)), 0);
    log.info("[:{d}] Initialized", .{port});
    return Socket{ .fd = fd, .rng = .init(seed) };
}

pub fn close(self: *Socket) void {
    _ = linux.close(self.fd);
    log.info("[:{d}] Closed", .{self.port});
}

const SendError = error{ Unknown, InvalidSocket };

pub fn sendTo(self: *Socket, data: []const u8, to: Address) SendError!usize {
    if (self.rng.random().float(f32) < self.loss_rate) {
        return data.len;
    }
    const sent_bytes = linux.sendto(self.fd, data.ptr, data.len, 0, @ptrCast(&to.sa), Address.sa_len);
    return switch (std.posix.errno(sent_bytes)) {
        .SUCCESS => sent_bytes,
        .BADF => SendError.InvalidSocket,
        else => SendError.Unknown,
    };
}

const ReceiveError = error{ InvalidArgument, Unknown };

const ReceiveResult = struct {
    packet: []u8,
    from: Address,
};

pub fn receive(self: *Socket, buffer: []u8) ReceiveError!?ReceiveResult {
    var sa_len: linux.socklen_t = Address.sa_len;
    var from: Address = undefined;
    var buffer_ptr = buffer.ptr;
    var buffer_len = buffer.len;

    const is_lost = self.rng.random().float(f32) < self.loss_rate;
    var fake_buffer: [4]u8 = undefined;
    if (is_lost) {
        buffer_ptr = &fake_buffer;
        buffer_len = fake_buffer.len;
    }

    const received_bytes = linux.recvfrom(self.fd, buffer_ptr, buffer_len, 0, @ptrCast(&from.sa), &sa_len);
    std.debug.assert(sa_len == @sizeOf(linux.sockaddr.in6));
    return switch (std.posix.errno(received_bytes)) {
        .SUCCESS => if (!is_lost) .{ .packet = buffer[0..received_bytes], .from = .{ .sa = from.sa } } else null,
        .AGAIN => null,
        .INVAL => ReceiveError.InvalidArgument,
        else => ReceiveError.Unknown,
    };
}

test "Socket: open, send/receive, close then send fails" {
    const addr = Address.initIp4(.{ 127, 0, 0, 1 }, 12601);
    var sender = try Socket.open(12600);
    var receiver = try Socket.open(12601);
    defer receiver.close();
    var buffer: [512]u8 = @splat(0);

    const sent = try sender.sendTo(&.{ 1, 2, 3, 4 }, addr);
    try expectEqual(4, sent);
    if (try receiver.receive(&buffer)) |received| {
        try expectEqual(4, received.packet.len);
    } else unreachable;

    sender.close();
    try expectError(error.InvalidSocket, sender.sendTo(&.{ 4, 3, 2, 1 }, addr));
}
