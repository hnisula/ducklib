#pragma once
#include <cstdint>

namespace DuckLib::Utility
{
void CopyText(char* source, char* dest, uint32_t destSize);
void WideToMultiByteText(const wchar_t* source, char* dest, uint32_t destSize);

void DebugOutput(const char* message);
}
