#pragma once

#include <cstdint>
#include <map>

#include <glm/glm.hpp>

class Shader {
private:
	uint32_t m_ProgramId;

	std::map<const char*, uint32_t> m_UniformLocations;

	uint32_t GetUniformLocation(const char* uniform);

	void CheckShaderErrors(uint32_t id);
	void CheckProgramErrors(uint32_t id);
public:
	Shader(const char* vertex, const char* fragment);
	~Shader();

	void Bind();

	void SetInt(const char* uniform, int value);
	void SetBool(const char* uniform, bool value);
	void SetFloat(const char* uniform, float value);
	void SetVec2(const char* uniform, glm::vec2 value);
	void SetVec3(const char* uniform, glm::vec3 value);
	void SetVec4(const char* uniform, glm::vec4 value);
	void SetMat4(const char* uniform, glm::mat4 value);
};