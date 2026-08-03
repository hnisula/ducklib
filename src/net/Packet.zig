const std = @import("std");
const builtin = @import("builtin");

pub const Packet = union(enum) {
    connect: Connect,
    accept: Accept,
    reject: Reject,

    const Connect = struct {
        fn write(self: *const Connect, buffer: []u8) !usize {
            _ = self;
            std.mem.writeInt(u32, buffer[0..4], 0xcc, builtin.cpu.arch.endian());
            return @sizeOf(u32);
        }

        fn read(buffer: []const u8) !Packet {
            const dummy = std.mem.readInt(u32, buffer[0..4], builtin.cpu.arch.endian());
            std.debug.assert(dummy == 0xcc);
            return .{ .connect = .{} };
        }
    };
    const Accept = struct {
        fn write(self: *const Accept, buffer: []u8) !usize {
            _ = self;
            std.mem.writeInt(u32, buffer[0..4], 0x1, builtin.cpu.arch.endian());
            return @sizeOf(u32);
        }

        fn read(buffer: []const u8) !Packet {
            const dummy = std.mem.readInt(u32, buffer[0..4], builtin.cpu.arch.endian());
            std.debug.assert(dummy == 0x1);
            return .{ .accept = .{} };
        }
    };
    const Reject = struct {
        reason: []const u8,

        fn write(self: *const Reject, buffer: []u8) !usize {
            const size = try std.fmt.bufPrintZ(buffer, "{s}", .{self.reason});
            return size.len;
        }

        fn read(buffer: []const u8) !Packet {
            const reason = std.mem.sliceTo(buffer.ptr, 0);
            return .{ .reject = .{ .reason = reason } };
        }
    };

    pub fn write(self: *const Packet, buffer: []u8) !usize {
        const header_size = 4;
        const tag: u8 = @intFromEnum(std.meta.activeTag(self.*));
        buffer[0] = tag;
        switch (self.*) {
            inline else => |payload| {
                const size = try payload.write(buffer[4..]);
                return header_size + size;
            },
        }
    }

    pub fn read(buffer: []const u8) !Packet {
        const tag: std.meta.Tag(Packet) = @enumFromInt(buffer[0]);
        return switch (tag) {
            .connect => try Connect.read(buffer[4..]),
            .accept => try Accept.read(buffer[4..]),
            .reject => try Reject.read(buffer[4..]),
        };
    }

    fn endian() std.cpu.arch.endian {
        return std.cpu.arch.endian();
    }
};
