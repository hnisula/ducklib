add_library(ducklib-core STATIC
        include/ducklib/core/logging/logger.h
        include/ducklib/core/platform/app_window.h
        include/ducklib/core/win_app_window.h
        src/logger.cpp
        src/platform/win_app_window.cpp
        include/ducklib/core/math.h
        src/math.cpp
        include/ducklib/core/logging/log_level.h
        include/ducklib/core/unicode.h
        src/unicode.cpp
        include/ducklib/core/platform/platform.h
        src/platform/linux_platform.cpp
        src/platform/win_platform.cpp
        src/platform/glfw_app_window.cpp
        include/ducklib/core/shared.h)

target_link_libraries(ducklib-core PRIVATE ducklib-compile-flags)
target_include_directories(ducklib-core PUBLIC include)