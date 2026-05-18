const std = @import("std");

pub fn main() !void {
    const my_int: u32 = 2000;
    var changing_int: u32 = 0;
    std.debug.print("Hello zig\n", .{});
    std.debug.print("My int: {d}", .{my_int});
    changing_int += 200;
    std.debug.print("Changed int: {d}", .{changing_int});
    const f: u32 = my_int + changing_int;
    std.debug.print("{d:200}", .{f});
}
