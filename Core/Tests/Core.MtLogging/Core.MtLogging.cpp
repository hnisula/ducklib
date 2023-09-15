#include <thread>
#include "../../Logging/Logger.h"

constexpr int THREAD_COUNT = 12;
constexpr int ITERATIONS_PER_THREAD = 40;
constexpr ducklib::LogLevel LOG_LEVEL = ducklib::LogLevel::WARNING;

void ThreadFunc()
{
	ducklib::Logger log;
	log.SetLogLevel(LOG_LEVEL);
	int accumulator = 0;

	for (int i = 0; i < ITERATIONS_PER_THREAD; ++i)
	{
		const auto level = (ducklib::LogLevel)(accumulator++ % 4);
		log.Log(level, "hello");
	}
}

int main()
{
	std::thread threads[THREAD_COUNT];

	for (auto& i : threads)
		i = std::thread(ThreadFunc);

	for (auto& thread : threads)
		thread.join();

	return 0;
}
