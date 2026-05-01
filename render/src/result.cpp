#include <cassert>

#include "ducklib/render/result.h"

namespace ducklib::render {
const char* to_string(Result result) {
    switch (result) {
    case Result::SUCCESS: return "Success";
    case Result::INSUFFICIENT_SPACE: return "Insufficient space";
    case Result::EXTENSION_NOT_FOUND: return "Extension not found";
    case Result::ERROR: return "Error";
    default: assert(false);
    }
}

Result map_hresult(HRESULT hresult) {
    switch (hresult) {
    case S_OK: return Result::SUCCESS;
    default: throw std::runtime_error("Unexpected hresult value when mapping");
    }
}
}
