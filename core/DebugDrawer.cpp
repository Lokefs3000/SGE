#include "pch.h"
#include "DebugDrawer.h"

#include <algorithm>

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ArialFont.h"
#include "Shader.h"

#include "resdata/res_shaders_debug_font_frag.h"
#include "resdata/res_shaders_debug_font_vert.h"

void DebugDrawer::drawChunk(uint32_t idx)
{
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, m_VertexChunks[idx].second, sizeof(DebugVertex) * m_VertexChunks[idx].first);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glDrawArrays(GL_TRIANGLES, 0, m_VertexChunks[idx].first);
	delete[] m_VertexChunks[idx].second;
}

DebugDrawer::DebugDrawer()
{
	glGenVertexArrays(1, &m_TextVAO);
	glGenBuffers(1, &m_TextVBO);

	glBindVertexArray(m_TextVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(DebugVertex) * DEBUG_VERTEX_SIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, Color));

	glGenTextures(1, &m_TextTex);
	glBindTexture(GL_TEXTURE_2D, m_TextTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int before = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &before);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ARIAL_FONT_WIDTH, ARIAL_FONT_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, arial_font);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, before);

	m_TextSdr = std::make_unique<Shader>(res_shaders_debug_font_vert, res_shaders_debug_font_frag);

	m_TextSdr->Bind();
	m_TextSdr->SetInt("uFont", 0);
	m_TextSdr->SetMat4("uProjection", glm::ortho(-480.0f, 480.0f, -360.0f, 360.0f, -1.0f, 1.0f));
}

DebugDrawer::~DebugDrawer()
{
	glDeleteTextures(1, &m_TextTex);
	glDeleteVertexArrays(1, &m_TextVAO);
	glDeleteBuffers(1, &m_TextVBO);
}

void DebugDrawer::text(std::string text, uint32_t color, int x, int y)
{
	float px = x;
	for (size_t i = 0; i < text.size(); i++)
	{
		if (m_VertexChunks.size() == 0 || m_VertexChunks[m_VertexChunks.size() - 1].first + 6 > DEBUG_VERTEX_SIZE) {
			m_VertexChunks.push_back(std::make_pair(0, new DebugVertex[DEBUG_VERTEX_SIZE]));
		}

		uint32_t pos = m_VertexChunks[m_VertexChunks.size() - 1].first;

		float r = (color & 0x00FF0000) / 16711680.0f;
		float g = (color & 0x0000FF00) / 65280.0f;
		float b = (color & 0x000000FF) / 255.0f;
		float a = 1.0f;

		FontGlyph glyph = arail_glyphs[text[i] - 31];

		float xpos = px;
		float ypos = y + glyph.originY - glyph.height;

		m_VertexChunks[m_VertexChunks.size() - 1].second[pos]     = { {xpos, ypos + glyph.height, glyph.uv1}, {r, g, b, a} };
		m_VertexChunks[m_VertexChunks.size() - 1].second[pos + 1] = { {xpos, ypos, glyph.uv1.x, glyph.uv2.y}, {r, g, b, a} };
		m_VertexChunks[m_VertexChunks.size() - 1].second[pos + 2] = { {xpos + glyph.width, ypos, glyph.uv2}, {r, g, b, a} };

		m_VertexChunks[m_VertexChunks.size() - 1].second[pos + 3] = { {xpos, ypos + glyph.height, glyph.uv1}, {r, g, b, a} };
		m_VertexChunks[m_VertexChunks.size() - 1].second[pos + 4] = { {xpos + glyph.width, ypos, glyph.uv2}, {r, g, b, a} };
		m_VertexChunks[m_VertexChunks.size() - 1].second[pos + 5] = { {xpos + glyph.width, ypos + glyph.height, glyph.uv2.x, glyph.uv1.y}, {r, g, b, a} };
	
		m_VertexChunks[m_VertexChunks.size() - 1].first += 6;

		px += glyph.advance;
	}
}

void DebugDrawer::draw()
{
	glBindVertexArray(m_TextVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextTex);

	m_TextSdr->Bind();

	for (size_t i = 0; i < m_VertexChunks.size(); i++)
	{
		drawChunk(i);
	}

	m_VertexChunks.clear();
}
