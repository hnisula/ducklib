cmake_minimum_required(VERSION 3.13)
project(Threading)

set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(_WIN32)
add_compile_definitions(DL_TRACK_ALLOCS)
add_library(Threading STATIC JobQueue.cpp Thread.cpp)