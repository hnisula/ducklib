#include "Utility.h"
#include <stdexcept>

#ifdef _WIN32
#include <hstring.h>
#endif

namespace ducklib::Utility
{
// TODO: Replace with some standard function
void WideToMultiByteText(const wchar_t* source, char* dest, uint32_t destSize)
{
	size_t convertedCount;
	errno_t error = wcstombs_s(&convertedCount, dest, destSize, source, destSize - 1);

	if (error != 0)
		throw std::runtime_error("Failed to convert from wide char string to multi-byte string");
}

void DebugOutput(const char* message)
{
#ifdef _WIN32
	OutputDebugStringA(message);
#endif
}
}
