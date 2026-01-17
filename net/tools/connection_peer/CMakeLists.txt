add_executable(ducklib-net-connection-peer
        connection_peer.cpp
)

target_link_libraries(ducklib-net-connection-peer
        ducklib-compile-flags
        ducklib-net
        ducklib-core
)