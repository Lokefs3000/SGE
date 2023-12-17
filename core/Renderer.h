#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>

class DataElement;
class Window;
class Texture;

struct RenderVertex {
	glm::vec4 Vertex;
	glm::vec4 Colors;
};

struct RenderChunk {
	std::vector<RenderVertex> Vertices;
	size_t At;
};

struct RenderMode {
	uint8_t Mode;
	size_t Start;
	uint32_t Chunk;

	Texture* Tex = NULL;
};

class Renderer {
private:
	void* m_OpenGL;

	std::vector<RenderChunk> m_Chunks;
	std::vector<RenderMode> m_Modes;

	uint32_t m_MainVertexArray;
	uint32_t m_MainVertexBuffer;

	RenderMode& ChangeMode(uint8_t mode, bool compareExtra);
	void AddRectToChunk(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
public:
	Renderer(std::weak_ptr<Window> window, std::shared_ptr<DataElement> data);
	~Renderer();

	void DrawSprite(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Texture* texture);

	void BeginFrame();
	void EndFrame();
};