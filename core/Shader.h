#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader {
private:
	uint32_t m_Program;

	std::unordered_map<std::string, uint32_t> m_UniformLocations;

	uint32_t GetUniformLocation(std::string name);

	void CheckShaderErrors(uint32_t id, bool isFragment);
	void CheckProgramErrors();
public:
	Shader(std::string vss, std::string fss);
	~Shader();

	void Bind();

	void SetInt(std::string name, int val);
	void SetBool(std::string name, bool val);
	void SetFloat(std::string name, float val);

	void SetVector2(std::string name, glm::vec2 val);
	void SetVector3(std::string name, glm::vec3 val);
	void SetVector4(std::string name, glm::vec4 val);

	void SetMatrix4(std::string name, glm::mat4 val);
};