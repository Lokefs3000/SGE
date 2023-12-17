#include "pch.h"
#include "Texture.h"

#include <glad/gl.h>

Texture::Texture(unsigned char* source, int width, int height, Filtering filtering, Repeating repeating, Format format)
{
	m_Width = width;
	m_Height = height;
	m_Repeating = repeating;
	m_Format = format;
	m_Filtering = filtering;

	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)filtering);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)repeating);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)repeating);

	glTexImage2D(GL_TEXTURE_2D, 0, (int)format, width, height, 0, (int)format, GL_UNSIGNED_BYTE, source);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_TextureId);
}

void Texture::Bind(uint8_t id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

uint32_t Texture::GetWidth()
{
	return m_Width;
}

uint32_t Texture::GetHeight()
{
	return m_Height;
}

Filtering Texture::GetFiltering()
{
	return m_Filtering;
}

Repeating Texture::GetRepeating()
{
	return m_Repeating;
}

Format Texture::GetFormat()
{
	return m_Format;
}
