#include "pch.h"
#include "Shader.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "DebugLogger.h"

uint32_t Shader::GetUniformLocation(const char* uniform)
{
    if (m_UniformLocations.count(uniform))
        return m_UniformLocations[uniform];

    uint32_t loc = glGetUniformLocation(m_ProgramId, uniform);
    m_UniformLocations.insert(std::make_pair(uniform, loc));

    return loc;
}

void Shader::CheckShaderErrors(uint32_t id)
{
    int params;
    glGetShaderiv(id, GL_COMPILE_STATUS, &params);
    if (!params) {
        char buffer[512];
        glGetShaderInfoLog(id, 512, NULL, buffer);
        DEBUGLOG("Failed to compile shader, reason: ", buffer);
    }
}

void Shader::CheckProgramErrors(uint32_t id)
{
    int params;
    glGetProgramiv(id, GL_LINK_STATUS, &params);
    if (!params) {
        char buffer[512];
        glGetProgramInfoLog(id, 512, NULL, buffer);
        DEBUGLOG("Failed to link program, reason: ", buffer);
    }
}

Shader::Shader(const char* vertex, const char* fragment)
{
    uint32_t vs, fs;

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex, NULL);
    glCompileShader(vs);
    CheckShaderErrors(vs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment, NULL);
    glCompileShader(fs);
    CheckShaderErrors(fs);

    m_ProgramId = glCreateProgram();
    glAttachShader(m_ProgramId, vs);
    glAttachShader(m_ProgramId, fs);
    glLinkProgram(m_ProgramId);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(m_ProgramId);
}

void Shader::Bind()
{
    glUseProgram(m_ProgramId);
}

void Shader::SetInt(const char* uniform, int value)
{
    glUniform1i(GetUniformLocation(uniform), value);
}

void Shader::SetBool(const char* uniform, bool value)
{
    SetInt(uniform, value);
}

void Shader::SetFloat(const char* uniform, float value)
{
    glUniform1f(GetUniformLocation(uniform), value);
}

void Shader::SetVec2(const char* uniform, glm::vec2 value)
{
    glUniform2f(GetUniformLocation(uniform), value.x, value.y);
}

void Shader::SetVec3(const char* uniform, glm::vec3 value)
{
    glUniform3f(GetUniformLocation(uniform), value.x, value.y, value.z);
}

void Shader::SetVec4(const char* uniform, glm::vec4 value)
{
    glUniform4f(GetUniformLocation(uniform), value.x, value.y, value.z, value.w);
}

void Shader::SetMat4(const char* uniform, glm::mat4 value)
{
    glUniformMatrix4fv(GetUniformLocation(uniform), 1, GL_FALSE, glm::value_ptr(value));
}
