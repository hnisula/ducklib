const std = @import("std");
const builtin = @import("builtin");
const Address = @import("Address.zig");
const Connection = @import("Connection.zig");

pub fn nowNs() i64 {
    switch (builtin.os.tag) {
        .windows => {
            var timestamp: std.os.windows.LARGE_INTEGER = undefined;
            var freq: std.os.windows.LARGE_INTEGER = undefined;
            _ = std.os.windows.ntdll.RtlQueryPerformanceCounter(&timestamp);
            _ = std.os.windows.ntdll.RtlQueryPerformanceFrequency(&freq);
            return @divFloor(@as(i64, timestamp) * std.time.ns_per_s, @as(i64, freq));
        },
        .linux => {
            var ts: std.os.linux.timespec = undefined;
            const rc = std.os.linux.clock_gettime(.MONOTONIC, &ts);
            if (std.posix.errno(rc) != .SUCCESS) {
                unreachable;
            }
            return @as(i64, ts.sec) * std.time.ns_per_s + ts.nsec;
        },
        else => unreachable,
    }
}
