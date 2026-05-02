#pragma once
#include <dxc/WinAdapter.h>

namespace ducklib::render {
enum class Result {
    SUCCESS = 0,
    ERROR = 1,
    INSUFFICIENT_SPACE,
    EXTENSION_NOT_FOUND
};

const char* to_string(Result result);
Result map_hresult(HRESULT hresult);
}
