#include "pch.h"
#include "Renderer.h"

#include <algorithm>

#include <sdl3/SDL.h>
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderResources.h"
#include "Shader.h"
#include "SGELoader.h"

#include "exports.h"

#include "resdata/res_shaders_2d_rect_vert.h"
#include "resdata/res_shaders_2d_rect_frag.h"

RenderMode& Renderer::switchRenderMode(uint8_t id)
{
	RenderMode mode;

	if (m_Chunks.size() == 0 || m_Chunks[m_Chunks.size() - 1]->size + 6 >= RENDER_VERTEX_SIZE) {
		m_Chunks.push_back(std::make_unique<RenderChunk>());

		mode.id = id;
		mode.begin = m_Chunks[m_Chunks.size() - 1]->size;
		mode.chunk = m_Chunks.size() - 1;
		mode.end = 0;
		m_Modes.push_back(mode);
	}
	else if (m_Modes.size() == 0 || m_Modes[m_Modes.size() - 1].id != id) {
		mode.id = id;
		mode.begin = m_Chunks[m_Chunks.size() - 1]->size;
		mode.chunk = m_Chunks.size() - 1;
		mode.end = 0;
		m_Modes.push_back(mode);
	}

	return m_Modes[m_Modes.size() - 1];
}

Renderer::Renderer(SDL_Window* window, RenderResources* res)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	m_OpenGL = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, m_OpenGL);
	gladLoadGL(SDL_GL_GetProcAddress);
	SDL_GL_SetSwapInterval(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_VertexArray);
	glGenBuffers(1, &m_VertexBuffer);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * RENDER_VERTEX_SIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, color));

	m_Shaders.RectShader = std::make_unique<Shader>(res_shaders_2d_rect_vert, res_shaders_2d_rect_frag);

	m_2DProjection = glm::ortho(-400.0f, 400.0f, 300.0f, -300.0f, -1.0f, 1.0f);

	m_Shaders.RectShader->Bind();
	m_Shaders.RectShader->SetMat4("uProjection", m_2DProjection);

	m_Resources = res;
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &m_VertexArray);
	glDeleteBuffers(1, &m_VertexBuffer);

	m_Shaders.RectShader.reset();

	SDL_GL_DeleteContext(m_OpenGL);
}

void Renderer::clear(glm::vec4 color)
{
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawRect(glm::vec4 color, int x, int y, int w, int h)
{
	RenderMode& mode = switchRenderMode(0);
	RenderChunk* chunk = m_Chunks[m_Chunks.size() - 1].get();

	mode.shader = m_Shaders.RectShader.get();
	mode.shaderUse = true;

	chunk->vertices[chunk->size] = { glm::vec4(x, y + h, 0.0f, 0.0f), color };
	chunk->vertices[chunk->size + 1] = { glm::vec4(x, y, 0.0f, 1.0f), color };
	chunk->vertices[chunk->size + 2] = { glm::vec4(x + w, y, 1.0f, 1.0f), color };

	chunk->vertices[chunk->size + 3] = { glm::vec4(x, y + h, 0.0f, 0.0f), color };
	chunk->vertices[chunk->size + 4] = { glm::vec4(x + w, y, 1.0f, 1.0f), color };
	chunk->vertices[chunk->size + 5] = { glm::vec4(x + w, y + h, 1.0f, 0.0f), color };

	mode.end += 6;
	chunk->size += 6;
}

void Renderer::draw()
{
	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	
	uint32_t lastId = UINT32_MAX;
	for (size_t i = 0; i < m_Modes.size(); i++)
	{
		RenderMode& mode = m_Modes[i];
		RenderChunk& chunk = *m_Chunks[mode.chunk].get();

		if (lastId != mode.chunk) {
			void* mem = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(mem, chunk.vertices, chunk.size * sizeof(RenderVertex));
			glUnmapBuffer(GL_ARRAY_BUFFER);
			lastId = mode.chunk;
		}

		if (mode.shaderUse) {
			if (m_LastShader != mode.shader && mode.shader != NULL) {
				m_LastShader = mode.shader;
				mode.shader->Bind();
			}
		}

		if (mode.textureUse) {
			if (m_LastTexture != mode.texture && mode.texture != 0) {
				m_LastTexture = mode.texture;
				glBindTexture(GL_TEXTURE_2D, mode.texture);
			}
		}

		glDrawArrays(GL_TRIANGLES, mode.begin, mode.end);
	}

	m_Chunks.clear();
	m_Modes.clear();
	lastId = UINT32_MAX;
	m_LastShader = nullptr;
}
