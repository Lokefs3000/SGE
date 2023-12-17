#include "pch.h"
#include "Shader.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "DebugMessenger.h"
#include "StringUtility.h"

uint32_t Shader::GetUniformLocation(std::string name)
{
	uint32_t id = 0;

	if (m_UniformLocations.count(name)) {
		id = m_UniformLocations.at(name);
	}
	else {
		id = glGetUniformLocation(m_Program, name.c_str());
		m_UniformLocations.insert(std::make_pair(name, id));
	}

	return id;
}

void Shader::CheckShaderErrors(uint32_t id, bool isFragment)
{
	int ret = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &ret);

	if (!ret) {
		char buf[512];
		glGetShaderInfoLog(id, 512, NULL, buf);
		DebugError((isFragment ? "Fragment" : "Vertex"), " shader compile error occured! Message:\n", buf);
	}
}

void Shader::CheckProgramErrors()
{
	int ret = 0;
	glGetProgramiv(m_Program, GL_LINK_STATUS, &ret);

	if (!ret) {
		char buf[512];
		glGetProgramInfoLog(m_Program, 512, NULL, buf);
		DebugError("Shader program error occured! Message:\n", buf);
	}
}

Shader::Shader(std::string vss, std::string fss)
{
#ifdef __EMSCRIPTEN__
	vss = StringUtility::Replace(vss, "$out", "out");
	vss = StringUtility::Replace(vss, "$in", "in");

	fss = StringUtility::Replace(fss, "$out", "out");
	fss = StringUtility::Replace(fss, "$in", "in");

	fss = StringUtility::Replace(fss, "330 core\n", "300 es\nprecision mediump float;\n");
	vss = StringUtility::Replace(vss, "330 core\n", "300 es\nprecision mediump float;\n");
#else
	std::vector<std::string> vssp = StringUtility::SplitString(vss, "\n");
	vss = "#version 330 core\n#extension GL_ARB_separate_shader_objects : enable\n";
	uint32_t inI = 0;
	uint32_t outI = 0;
	for (size_t i = 1; i < vssp.size() - 1; i++)
	{
		if (vssp[i].find("$out") != std::string::npos) {
			vssp[i] = "layout(location = " + std::to_string(outI) + ") out " + vssp[i].substr(5);
			outI++;
		}
		else if (vssp[i].find("$in") != std::string::npos) {
			vssp[i] = "layout(location = " + std::to_string(inI) + ") in " + vssp[i].substr(4);
			inI++;
		}

		vss += vssp[i] + "\n";
	}

	std::vector<std::string> fssp = StringUtility::SplitString(fss, "\n");
	fss = "#version 330 core\n#extension GL_ARB_separate_shader_objects : enable\n";
	inI = 0;
	outI = 0;
	for (size_t i = 1; i < fssp.size() - 1; i++)
	{
		if (fssp[i].find("$out") != std::string::npos) {
			fssp[i] = "layout(location = " + std::to_string(outI) + ") out " + fssp[i].substr(5);
			outI++;
		}
		else if (fssp[i].find("$in") != std::string::npos) {
			fssp[i] = "layout(location = " + std::to_string(inI) + ") in " + fssp[i].substr(4);
			inI++;
		}

		fss += fssp[i] + "\n";
	}
#endif

	const char* vssc = vss.c_str();
	const char* fssc = fss.c_str();

	uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vssc, NULL);
	glCompileShader(vs);
	CheckShaderErrors(vs, false);

	uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fssc, NULL);
	glCompileShader(fs);
	CheckShaderErrors(fs, true);

	m_Program = glCreateProgram();
	glAttachShader(m_Program, vs);
	glAttachShader(m_Program, fs);
	glLinkProgram(m_Program);
	CheckProgramErrors();

	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::~Shader()
{
	glDeleteProgram(m_Program);
}

void Shader::Bind()
{
	glUseProgram(m_Program);
}

void Shader::SetInt(std::string name, int val)
{
	glUniform1i(GetUniformLocation(name), val);
}

void Shader::SetBool(std::string name, bool val)
{
	SetInt(name, (int)val);
}

void Shader::SetFloat(std::string name, float val)
{
	glUniform1f(GetUniformLocation(name), val);
}

void Shader::SetVector2(std::string name, glm::vec2 val)
{
	glUniform2f(GetUniformLocation(name), val.x, val.y);
}

void Shader::SetVector3(std::string name, glm::vec3 val)
{
	glUniform3f(GetUniformLocation(name), val.x, val.y, val.z);
}

void Shader::SetVector4(std::string name, glm::vec4 val)
{
	glUniform4f(GetUniformLocation(name), val.x, val.y, val.z, val.w);
}

void Shader::SetMatrix4(std::string name, glm::mat4 val)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(val));
}
