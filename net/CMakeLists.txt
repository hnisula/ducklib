cmake_minimum_required(VERSION 3.20)

project(ducklib-net)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} STATIC
        src/Net.cpp
        src/net_client.cpp
        src/Shared.cpp
        src/Socket.cpp
        src/net_client2.cpp
        ../include/net/net_client2.h
        ../include/net/net_buffer_writer.h
        src/net_buffer_writer.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ../include)
set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".lib")