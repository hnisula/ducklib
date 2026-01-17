add_library(ducklib-net STATIC
        src/net.cpp
        src/shared.cpp
        src/socket.cpp
        include/ducklib/net/connection.h
        src/connection.cpp
        include/ducklib/net/serialization.h
        src/serialization.cpp
)
target_include_directories(ducklib-net PUBLIC include)
target_link_libraries(ducklib-net PUBLIC
        ducklib-compile-flags
        ducklib-core
        $<$<PLATFORM_ID:Windows>:ws2_32>)

add_subdirectory(tools/connection_peer)