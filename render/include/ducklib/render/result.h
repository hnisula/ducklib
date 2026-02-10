#ifndef DUCKLIB_ERRORS_H
#define DUCKLIB_ERRORS_H

namespace ducklib::render {
enum class Result {
    SUCCESS = 0,
    ERROR = 1,
    INSUFFICIENT_SPACE,
    EXTENSION_NOT_FOUND
};

const char* to_string(Result result);
}

#endif //DUCKLIB_ERRORS_H