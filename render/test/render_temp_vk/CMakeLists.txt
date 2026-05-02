add_executable(ducklib-render-temp-vk
        temp_vk.cpp)

target_link_libraries(ducklib-render-temp-vk
        ducklib-compile-flags
        ducklib-render)