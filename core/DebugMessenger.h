#pragma once

#include <string>
#include <sstream>

class DebugMessenger {
private:
	static void InternalMsgBox(std::string title, std::string msg);
public:
	static const char* warnNames[];

	template<typename... Args>
	static void FatalMessageBox(std::string title, Args ...message);

	template<typename... Args>
	static void TemplatedArgLog(const char* func, uint32_t line, uint8_t sev, Args ...args);
	
	static void PrintStringSeverity(std::string message, uint8_t severity);
};

#define DebugInfo(...) DebugMessenger::TemplatedArgLog(__FUNCTION__, __LINE__, 0, __VA_ARGS__);
#define DebugWarning(...) DebugMessenger::TemplatedArgLog(__FUNCTION__, __LINE__, 1, __VA_ARGS__);
#define DebugError(...) DebugMessenger::TemplatedArgLog(__FUNCTION__, __LINE__, 2, __VA_ARGS__);
#define DebugFatal(...) DebugMessenger::TemplatedArgLog(__FUNCTION__, __LINE__, 3, __VA_ARGS__);

#define FatalError(title, ...) DebugMessenger::FatalMessageBox(title, __VA_ARGS__);

template<typename ...Args>
inline void DebugMessenger::FatalMessageBox(std::string title, Args ...message)
{
	std::ostringstream stream;

	([&]
		{
			stream << message;
		}(), ...);

	InternalMsgBox(title, stream.str());
}

template<typename ...Args>
inline void DebugMessenger::TemplatedArgLog(const char* func, uint32_t line, uint8_t sev, Args ...args)
{
	std::ostringstream stream;

	([&]
		{
			stream << args;
		}(), ...);

	std::string msg = "[" + std::string(func) + ":" + std::to_string(line) + "/" + warnNames[sev] + "]: " + stream.str();
	PrintStringSeverity(msg, sev);
}
