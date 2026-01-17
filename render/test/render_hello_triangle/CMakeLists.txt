add_executable(ducklib-render-test-hello-triangle WIN32
        render_hello_triangle.cpp)

target_link_libraries(ducklib-render-test-hello-triangle
        ducklib-compile-flags
        ducklib-render)

add_custom_command(TARGET ducklib-render-test-hello-triangle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders.hlsl"
        "$<TARGET_FILE_DIR:ducklib-render-test-hello-triangle>/shaders.hlsl"
        COMMENT "Copying shaders.hlsl to executable directory")
