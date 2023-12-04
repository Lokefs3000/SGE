#pragma once

#include <memory>
#include <functional>

#include <Guid.hpp>
#include <glm/glm.hpp>

struct SDL_Window;
class RenderResources;
class Shader;
class Texture;

#define RENDER_VERTEX_SIZE 512

struct RenderVertex {
	glm::vec4 vertex;
	glm::vec4 color;
};

struct RenderChunk {
	RenderVertex vertices[RENDER_VERTEX_SIZE];
	uint32_t size = 0;
};

struct RenderMode {
	uint8_t id;

	uint64_t chunk;
	uint64_t begin;
	uint64_t end;

	bool shaderUse = false;
	bool textureUse = false;

	Shader* shader;
	uint32_t texture;
};

struct RenderShaders {
	std::unique_ptr<Shader> RectShader;
};

class Renderer {
private:
	void* m_OpenGL;
	RenderResources* m_Resources;

	Shader* m_LastShader = nullptr;
	uint32_t m_LastTexture = UINT32_MAX;
	std::vector<std::unique_ptr<RenderChunk>> m_Chunks;
	std::vector<RenderMode> m_Modes;

	glm::mat4 m_2DProjection;

	RenderShaders m_Shaders;

	uint32_t m_VertexArray;
	uint32_t m_VertexBuffer;

	RenderMode& switchRenderMode(uint8_t id);
public:
	const xg::Guid DEAD_GUID = xg::Guid("000000000000000");
	const xg::Guid AUTO_GUID = xg::Guid("fffffffffffffff");

	Renderer(SDL_Window* window, RenderResources* res);
	~Renderer();

	void clear(glm::vec4 color);
	void drawRect(glm::vec4 color, int x, int y, int w, int h);

	void draw();
};