include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(ducklib_platform_unit_tests
        file_unit_tests.cpp
)

target_link_libraries(ducklib_platform_unit_tests
        ducklib-compile-flags
        GTest::gtest_main
        ducklib-platform
        $<$<PLATFORM_ID:Windows>:Ws2_32>
)
target_include_directories(ducklib_platform_unit_tests PUBLIC include)

include(GoogleTest)
gtest_discover_tests(ducklib_platform_unit_tests)
