#include "pch.h"
#include "DebugMessenger.h"

#ifdef _WIN32
#include <Windows.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#elif __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/console.h>
#endif

#include <iostream>

const char* DebugMessenger::warnNames[] = {
	"Info",
	"Warn",
	"Err",
	"Fatal"
};

#ifdef _WIN32
int warnColor[] = {
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_RED,
	BACKGROUND_RED
};
#endif


void DebugMessenger::InternalMsgBox(std::string title, std::string message)
{
	DebugFatal(title, " : ", message);
#ifdef _WIN32
	MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_TASKMODAL);
	exit(-1);
#elif __EMSCRIPTEN__
	std::string text = "alert(\"" + title + "\\n\\nMessage: " + message + "\");";
	emscripten_run_script(text.c_str());
	emscripten_force_exit(-1);
#endif
}

void DebugMessenger::PrintStringSeverity(std::string message, uint8_t severity)
{
#ifdef _WIN32
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, warnColor[severity]);
	std::cout << message << std::endl;
#elif __EMSCRIPTEN__
	switch (severity)
	{
	case 0:
		emscripten_console_log(message.c_str());
		break;
	case 1:
		emscripten_console_warn(message.c_str());
		break;
	case 2:
		emscripten_console_error(message.c_str());
		break;
	default:
		break;
	}
#endif
}
