#include "pch.h"
#include "DebugLogger.h"

#include <glad/gl.h>

static void GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::string source_s;
	std::string type_s;
	std::string severity_s;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API_ARB:
		source_s = "SourceAPI";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
		source_s = "WindowSystem";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
		source_s = "ShaderCompiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
		source_s = "ThirdParty";
		break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:
		source_s = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER_ARB:
		source_s = "Other";
		break;
	default:
		source_s = "Unkown";
		break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB:
		type_s = "APIError";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
		type_s = "DepractedBehavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
		type_s = "UndefinedBehavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
		type_s = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:
		type_s = "Performance";
		break;
	case GL_DEBUG_TYPE_OTHER_ARB:
		type_s = "Other";
		break;
	default:
		type_s = "Unkown";
		break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW_ARB:
		severity_s = "Low";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		severity_s = "Medium";
		break;
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		severity_s = "High";
		break;
	default:
		return;
	}

	DEBUGLOG(severity_s, " severity reported from ", source_s, " with type of ", type_s, " sent gl message: ", message);
}

void DebugLogger::EnableGLLogging()
{
	glEnable(GL_DEBUG_CALLBACK_FUNCTION_ARB);
	glDebugMessageCallbackARB(GLErrorCallback, NULL);
}