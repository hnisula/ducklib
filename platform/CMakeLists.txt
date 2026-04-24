add_library(ducklib-platform STATIC
        include/ducklib/platform/file.h
        src/file.cpp)

target_link_libraries(ducklib-platform PRIVATE ducklib-compile-flags)
target_include_directories(ducklib-platform PUBLIC include)