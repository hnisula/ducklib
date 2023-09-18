#pragma once
#include <cstdint>

namespace ducklib::Utility
{
void WideToMultiByteText(const wchar_t* source, char* dest, uint32_t destSize);

void DebugOutput(const char* message);
}
