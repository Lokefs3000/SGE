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

#include <glad/gl.h>

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

void DebugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void DebugMessenger::SetupDebugMessaging()
{
#ifdef __EMSCRIPTEN__
	DebugWarning("Debug output requested but not available in WebGL!");
#else
	glDebugMessageCallbackARB(DebugCallbackARB, NULL);
#endif // __EMSCRIPTEN__
}

void DebugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (severity != GL_DEBUG_SEVERITY_LOW_ARB) {
		std::string source_str;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API_ARB:
			source_str = "OpenGL";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
			source_str = "OpenGL";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
			source_str = "ShaderC";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
			source_str = "TrdParty";
			break;
		case GL_DEBUG_SOURCE_APPLICATION_ARB:
			source_str = "App";
			break;
		case GL_DEBUG_SOURCE_OTHER_ARB:
		default:
			source_str = "Other";
			break;
		}

		std::string type_str;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR_ARB:
			type_str = "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
			type_str = "Depracted";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
			type_str = "UndefBehavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY_ARB:
			type_str = "Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE_ARB:
			type_str = "Performance";
			break;
		case GL_DEBUG_TYPE_OTHER_ARB:
		default:
			type_str = "Other";
			break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_MEDIUM_ARB:
			DebugWarning("\"", source_str, "\" generated an warning message with type: \"", type_str, "\", message: ", message);
			break;
		case GL_DEBUG_SEVERITY_HIGH_ARB:
			DebugError("\"", source_str, "\" generated an error message with type: \"", type_str, "\", message: ", message);
			break;
		default:
			break;
		}
	}
}