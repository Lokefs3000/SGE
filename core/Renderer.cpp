#include "pch.h"
#include "Renderer.h"

#include <SDL3/SDL.h>
#include <glad/gl.h>

#include "DataParser.h"
#include "Window.h"
#include "Texture.h"

RenderMode& Renderer::ChangeMode(uint8_t mode, bool compareExtra)
{
	if (m_Chunks.size() == 0 || m_Chunks[m_Chunks.size() - 1].At + 6 > RND_BUFSIZE) {
		RenderChunk chunk;
		chunk.At = 0;
		chunk.Vertices.resize(RND_BUFSIZE * sizeof(RenderVertex));
		m_Chunks.push_back(chunk);
	}
	if (m_Modes.size() == 0) {
		RenderMode rmode{};
		rmode.Chunk = m_Chunks.size() - 1;
		rmode.Mode = mode;
		rmode.Start = 0;
		m_Modes.push_back(rmode);
	}
	else if (m_Modes[m_Modes.size() - 1].Mode != mode || !compareExtra) {
		RenderMode rmode{};
		rmode.Chunk = m_Chunks.size() - 1;
		rmode.Mode = mode;
		rmode.Start = 0;
		m_Modes.push_back(rmode);
	}

	return m_Modes[m_Modes.size() - 1];
}

void Renderer::AddRectToChunk(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
	RenderChunk* chunk = &m_Chunks[m_Chunks.size() - 1];

	glm::vec2 half = size / 2.0f;

	chunk->Vertices[chunk->At]	   = { glm::vec4(pos.x - half.x, pos.y + half.y, 0.0f, 0.0f), color };
	chunk->Vertices[chunk->At + 1] = { glm::vec4(pos.x - half.x, pos.y - half.y, 0.0f, 1.0f), color };
	chunk->Vertices[chunk->At + 2] = { glm::vec4(pos.x + half.x, pos.y - half.y, 1.0f, 1.0f), color };

	chunk->Vertices[chunk->At + 3] = { glm::vec4(pos.x - half.x, pos.y + half.y, 0.0f, 0.0f), color };
	chunk->Vertices[chunk->At + 4] = { glm::vec4(pos.x + half.x, pos.y - half.y, 1.0f, 1.0f), color };
	chunk->Vertices[chunk->At + 5] = { glm::vec4(pos.x + half.x, pos.y + half.y, 1.0f, 0.0f), color };

	chunk->At += 6;
}

Renderer::Renderer(std::weak_ptr<Window> window, std::shared_ptr<DataElement> data)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifndef NDEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	std::shared_ptr<Window> lock = window.lock();
	m_OpenGL = SDL_GL_CreateContext(lock->GetInternalWindow());
	SDL_GL_MakeCurrent(lock->GetInternalWindow(), m_OpenGL);
	gladLoadGL(SDL_GL_GetProcAddress);

	std::shared_ptr<DataElement> rendConf = data->GetChild("config")->GetChild("renderer");
	SDL_GL_SetSwapInterval(rendConf->GetValue("vsync").ValInt);

	glGenVertexArrays(1, &m_MainVertexArray);
	glGenBuffers(1, &m_MainVertexBuffer);

	glBindVertexArray(m_MainVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_MainVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * 6 * RND_BUFSIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, Vertex));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, Colors));
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &m_MainVertexArray);
	glDeleteBuffers(1, &m_MainVertexBuffer);

	gladLoaderUnloadGL();
	SDL_GL_DeleteContext(m_OpenGL);
}

void Renderer::DrawSprite(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Texture* texture)
{
	RenderMode& mode = ChangeMode(0, (m_Modes.size() > 0 && m_Modes.end()._Ptr->Tex == texture));
	mode.Tex = texture;
	AddRectToChunk(pos, size, color);
}

void Renderer::BeginFrame()
{
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::EndFrame()
{
	glBindVertexArray(m_MainVertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_MainVertexBuffer);
	
	size_t activeChunk = UINT64_MAX;

	for (size_t i = 0; i < m_Modes.size(); i++)
	{
		RenderMode& mode = m_Modes[i];
		RenderChunk& chunk = m_Chunks[mode.Chunk];

		if (mode.Chunk != activeChunk) {
			void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(ptr, chunk.Vertices.data(), chunk.At);
			glUnmapBuffer(GL_ARRAY_BUFFER);

			activeChunk = mode.Chunk;
		}

		uint32_t count = 0;

		if (m_Modes.size() > i + 1)
			count = m_Modes[i + 1].Start - mode.Start;
		else
			count = chunk.At - mode.Start;

		switch (mode.Mode)
		{
		case 0:
			if (mode.Tex != NULL) {
				mode.Tex->Bind(0);
			}
			break;
		default:
			break;
		}

		glDrawArrays(GL_TRIANGLES, mode.Start, count);
	}

	m_Modes.clear();
	m_Chunks.clear();
}
