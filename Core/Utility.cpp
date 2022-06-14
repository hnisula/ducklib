#include "Utility.h"

#ifdef _WIN32
#include <hstring.h>
#endif

namespace DuckLib
{
void DebugOutput(const char* message)
{
#ifdef _WIN32
	OutputDebugStringA(message);
#endif
}
}
