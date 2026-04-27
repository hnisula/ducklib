#include <filesystem>
#include <fstream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "third_party/doctest.h"
#include "ducklib/platform/file.h"

using namespace ducklib;

struct TestFixture {
    std::filesystem::path temp_dir_path;
    std::filesystem::path temp_file_path;

    TestFixture() {
        temp_dir_path = std::filesystem::temp_directory_path() / "ducklib-file-tests";
        std::filesystem::create_directory(temp_dir_path);
        temp_file_path = temp_dir_path / "testfile.txt";
        std::ofstream file(temp_file_path);
        file << "int first = 0;";
    }

    ~TestFixture() {
        std::filesystem::remove_all(temp_dir_path);
    }
};

TEST_CASE_FIXTURE(TestFixture, "Can create file") {
    File test_file;
    auto filename = "testfile.txt";
    test_file.open(filename, FileMode::WRITE);
    CHECK(std::filesystem::exists(temp_dir_path / filename));
}

TEST_CASE_FIXTURE(TestFixture, "Basic file operations") {
    File file;
    file.open(temp_file_path.generic_string(), FileMode::READ);
    std::array<std::byte, 1024> text_buffer{};
    file.read_all(text_buffer);
    auto str = std::string((const char*)text_buffer.data());
    auto expected = std::string("int first = 0;");
    CHECK(str == expected);
}
