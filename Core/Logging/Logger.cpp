#include "Logger.h"

#include <iostream>

namespace ducklib
{
std::mutex Logger::stdoutMutex;

int Logger::AddPrefix(char* buffer, LogLevel level)
{
	static const char* prefixes[] = {
		"ERROR", "WARNING", "INFO", "DEBUG"
	};

	// Level prefix
	char* it = buffer;
	const char* levelPrefix = prefixes[(int)level];
	int prefixLength = (int)strlen(levelPrefix);

	strncpy(it, levelPrefix, prefixLength);
	it += prefixLength;

	// Prefix delimiter
	constexpr char prefixDelimiter[] = ": ";
	constexpr int prefixDelimiterLength = 2;
	strncpy(it, prefixDelimiter, prefixDelimiterLength);
	prefixLength += prefixDelimiterLength;

	return prefixLength;
}

void Logger::StdoutSink(const char* text)
{
	stdoutMutex.lock();
	std::cout << text << std::endl;
	stdoutMutex.unlock();
}
}
