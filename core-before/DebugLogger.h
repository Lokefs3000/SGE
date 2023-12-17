#pragma once

#include <cstdint>
#include <sstream>
#include <iostream>

class DebugLogger {
private:

public:
	template<typename... Args>
	static void Log(uint64_t line, const char* func, Args... args);

	static void EnableGLLogging();
};

template<typename ...Args>
inline void DebugLogger::Log(uint64_t line, const char* func, Args ...args)
{
	std::ostringstream stream;

	([&]
		{
			stream << args;
		}(), ...);

	std::cout << "[" << func << ":" << line << "]: " << stream.str() << std::endl;
}

#if _DEBUG
#define DEBUGLOG(...) DebugLogger::Log(__LINE__, __FUNCTION__, __VA_ARGS__);
#else
#define DEBUGLOG(...)
#endif