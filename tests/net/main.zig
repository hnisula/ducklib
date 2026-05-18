const std = @import("std");
const expect = std.testing.expect;

test "simple test" {
    const value: u32 = 100;
    try expect(100 == value);
}
