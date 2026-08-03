const std = @import("std");

const Connection = @import("Connection.zig");
const Packet = @import("Packet.zig").Packet;
const Socket = @import("Socket.zig");
const Address = @import("Address.zig");
const shared = @import("shared.zig");

const log = std.log.scoped(.server);

const Server = @This();
socket: Socket,
conns: [max_conns]Connection,
addrs: [max_conns]Address, // TODO: Remove
conn_count: u8 = 0,

const max_conns = 16;

pub fn init(port: u16) !Server {
    log.info("Server initialized on port {d}", .{port});
    return .{ .socket = try Socket.open(port), .conns = @splat(.{}), .addrs = @splat(.invalid) };
}

pub fn deinit(self: *Server) void {
    self.socket.close();
    log.info("Server shut down", .{});
}

pub fn tick(self: *Server) !void {
    var recv_buffer: [Socket.MTU]u8 = undefined;
    var send_buffer: [Socket.MTU]u8 = undefined;

    while (try self.socket.receive(&recv_buffer)) |received_packet| {
        const received = try Packet.read(received_packet.packet);
        const existing_conn = self.findConn(received_packet.from);

        if (existing_conn) |conn| {
            const effect = conn.handle(.{ .packet = received });
            if (effect) |e| {
                switch (e) {
                    .send_packet => |send_packet| switch (send_packet) {
                        .accept => {
                            const size = try send_packet.write(&recv_buffer);
                            _ = try self.socket.sendTo(send_buffer[0..size], received_packet.from);
                        },
                        else => {},
                    },
                }
            }
        } else {
            if (received == .connect) {
                if (self.conn_count < max_conns) {
                    const new_count = self.conn_count + 1;
                    self.conns[new_count] = try .init(received_packet.from);
                    self.addrs[new_count] = received_packet.from;
                    self.conn_count = new_count;
                    log.info("Server added connection {d} as connected", .{self.conns[new_count].id});
                    var conn = &self.conns[new_count];
                    conn.state = .connected;
                    const accept_packet: Packet = .{ .accept = .{} };
                    const packet_size = try accept_packet.write(&send_buffer);
                    _ = try self.socket.sendTo(send_buffer[0..packet_size], self.addrs[new_count]);
                }
            }
        }
    }
}

fn findConn(self: *Server, addr: Address) ?*Connection {
    for (0..self.conn_count) |i| {
        if (self.addrs[i].eq(&addr)) {
            return &self.conns[i];
        }
    }

    return null;
}
