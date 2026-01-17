#include <cassert>

#include "ducklib/render/result.h"

namespace ducklib::render {
const char* to_string(Result result) {
    switch (result) {
    case Result::SUCCESS: return "Success";
    case Result::ERROR: return "Error";
    default: assert(false);
    }
}
}
