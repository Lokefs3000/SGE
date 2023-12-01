#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

struct DebugVertex {
	glm::vec4 Vertex;
	glm::vec4 Color;
};

#define DEBUG_VERTEX_SIZE 512 * 6

class Shader;

class DebugDrawer {
private:
	std::vector<std::pair<uint32_t, DebugVertex*>> m_VertexChunks;

	uint32_t m_TextVAO;
	uint32_t m_TextVBO;
	uint32_t m_TextTex;

	std::unique_ptr<Shader> m_TextSdr;

	void drawChunk(uint32_t idx);
public:
	DebugDrawer();
	~DebugDrawer();

	void text(std::string text, uint32_t color, int x, int y);
	void draw();
};