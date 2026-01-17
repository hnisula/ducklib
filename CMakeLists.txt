cmake_minimum_required(VERSION 3.31)
project(ducklib)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(ducklib-compile-flags INTERFACE)

target_compile_options(ducklib-compile-flags INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi /Od>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Debug>>:-g -O0>
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang>,$<CONFIG:Release>>:-O2>
)

target_compile_definitions(ducklib-compile-flags INTERFACE
        $<$<CONFIG:Release>:NDEBUG>
)

add_subdirectory(core)
add_subdirectory(net)
add_subdirectory(net/test/net_shared_unit_tests)
add_subdirectory(render)
add_subdirectory(input)

#add_subdirectory(threading)
