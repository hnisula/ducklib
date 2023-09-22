#pragma once
#include <cstdint>
#include <mutex>

namespace ducklib
{
enum class LogLevel
{
	ERROR,
	WARNING,
	INFO,
	DEBUG,
};

class Logger
{
public:
	void SetLogLevel(LogLevel level);

	template <typename... Args>
	void Error(const char* text, const Args&&... args);
	template <typename... Args>
	void Warn(const char* text, const Args&&... args);
	template <typename... Args>
	void Info(const char* text, const Args&&... args);
	template <typename... Args>
	void Debug(const char* text, const Args&&... args);

	template <typename... Args>
	void Log(LogLevel level, const char* text, const Args&&... args);

private:
	static constexpr uint16_t MESSAGE_FORMAT_BUFFER_SIZE = 512;

	static std::mutex stdoutMutex;

	/**
	 * \return Prefix length in chars
	 */
	static int AddPrefix(char* buffer, LogLevel level);
	static void StdoutSink(const char* text); // TODO: Move to somewhere external

	LogLevel setLevel = LogLevel::WARNING;
};

inline void Logger::SetLogLevel(LogLevel level)
{
	setLevel = level;
}


template <typename... Args>
void Logger::Error(const char* text, const Args&&... args)
{
	Log(LogLevel::ERROR, text, std::forward<const Args&&>(args)...);
}

template <typename... Args>
void Logger::Warn(const char* text, const Args&&... args)
{
	Log(LogLevel::WARNING, text, std::forward<const Args&&>(args)...);
}

template <typename... Args>
void Logger::Info(const char* text, const Args&&... args)
{
	Log(LogLevel::INFO, text, std::forward<const Args&&>(args)...);
}

template <typename... Args>
void Logger::Debug(const char* text, const Args&&... args)
{
	Log(LogLevel::DEBUG, text, std::forward<const Args&&>(args)...);
}

template <typename... Args>
void Logger::Log(LogLevel level, const char* text, const Args&&... args)
{
	if ((int)level > (int)setLevel) // TODO: Come up with better names
		return;

	char formatBuffer[MESSAGE_FORMAT_BUFFER_SIZE];
	const int prefixLength = AddPrefix(formatBuffer, level);
	snprintf(&formatBuffer[prefixLength], sizeof formatBuffer - prefixLength, text, std::forward<const Args&&>(args)...);

	// TODO: Support sinks/outputs
	StdoutSink(formatBuffer);
}
}
