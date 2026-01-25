add_library(ducklib-input STATIC
        include/ducklib/input/input.h
        src/input_win.cpp
        src/input_unix.cpp)

target_link_libraries(ducklib-input PRIVATE ducklib-compile-flags)
target_include_directories(ducklib-input PUBLIC include)