add_executable(ducklib-render-test-hello-gui WIN32
        render_hello_gui.cpp)

target_link_libraries(ducklib-render-test-hello-gui
        ducklib-compile-flags
        ducklib-render
        ducklib-input)

add_custom_command(TARGET ducklib-render-test-hello-gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders.hlsl"
        "$<TARGET_FILE_DIR:ducklib-render-test-hello-gui>/shaders.hlsl"
        COMMENT "Copying shaders.hlsl to executable directory")
add_custom_command(TARGET ducklib-render-test-hello-gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/gui_shaders.hlsl"
        "$<TARGET_FILE_DIR:ducklib-render-test-hello-gui>/gui_shaders.hlsl"
        COMMENT "Copying gui_shaders.hlsl to executable directory")
add_custom_command(TARGET ducklib-render-test-hello-gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/text_shaders.hlsl"
        "$<TARGET_FILE_DIR:ducklib-render-test-hello-gui>/text_shaders.hlsl"
        COMMENT "Copying text_shaders.hlsl to executable directory")