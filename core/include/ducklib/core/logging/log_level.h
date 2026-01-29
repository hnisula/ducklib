#pragma once
#include <cstdint>
#include <cassert>

#undef ERROR

namespace ducklib {
enum class LogLevel : uint8_t {
    OFF,
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE
};

inline const char* to_string(LogLevel level) {
    switch (level) {
    case LogLevel::OFF: return "OFF";
    case LogLevel::FATAL: return "FATAL";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::INFO: return "INFO";
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::TRACE: return "TRACE";
    }
    
    assert(false);
}
}
