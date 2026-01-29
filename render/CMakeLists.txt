add_library(ducklib-render STATIC
        src/rhi/d3d12/rhi.cpp
        include/ducklib/render/rhi/d3d12/rhi.h
        include/ducklib/render/rhi/d3d12/device.h
        src/rhi/d3d12/device.cpp
        include/ducklib/render/rhi/d3d12/constants.h
        include/ducklib/render/rhi/d3d12/types.h
        include/ducklib/render/rhi/d3d12/resource_manager.h
        src/rhi/d3d12/resource_manager.cpp
        include/ducklib/render/rhi/d3d12/shader.h
        src/rhi/d3d12/shader.cpp
        src/rhi/d3d12/types.cpp
        include/ducklib/render/rhi/d3d12/render_util.h
#        include/ducklib/render/gui/font.h
#        src/gui/font.cpp
        src/rhi/d3d12/render_util.cpp
#        include/ducklib/render/gui/gui.h
#        src/gui/gui.cpp
        include/ducklib/render/rhi/vk.h
        src/rhi/vk/vk.cpp
        include/ducklib/render/rhi/shared.h
        src/rhi/vk/vk_util.h
        include/ducklib/render/result.h
        src/rhi/vk/vk_util.cpp
        src/render_util.h
        src/render_util.cpp
        src/result.cpp
        include/ducklib/render/rhi/vk.h
        include/ducklib/render/rhi/rhi.h
        include/ducklib/render/rhi/vk/shared_vk.h)

option(D3D12_RENDERER "Use D3D12 renderer instead of Vulkan" OFF)

if(D3D12_RENDERER)
    target_compile_definitions(ducklib-render PUBLIC DL_D3D12)
    target_link_libraries(ducklib-render
            PUBLIC
            ducklib-compile-flags
            ducklib-core
            d3d12
            dxgi
            dxguid
            d3dcompiler)
else()
    target_compile_definitions(ducklib-render PUBLIC DL_VK)
    target_link_libraries(ducklib-render
            ducklib-compile-flags
            ducklib-core
            vulkan
            glfw)
endif()

target_include_directories(ducklib-render PUBLIC include)

add_custom_command(TARGET ducklib-render POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Roboto-Regular.ttf"
        "$<TARGET_FILE_DIR:ducklib-render>/Roboto-Regular.ttf"
        COMMENT "Copying Roboto-Regular.ttf to executable directory")

add_subdirectory(test/render_hello_triangle)
add_subdirectory(test/render_hello_gui)
add_subdirectory(test/render_temp_vk)