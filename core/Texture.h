#pragma once

#include <string>

enum class Filtering {
	BiLinear = 0x2601,
	Nearest = 0x2600
};

enum class Repeating {
	Clamp = 0x812F,
	Repeat = 0x2901
};

enum class Format {
	R = 0x1903,
	RG = 0x8227,
	RGB = 0x1907,
	RGBA = 0x1908
};

class Texture {
private:
	uint32_t m_TextureId;

	uint32_t m_Width;
	uint32_t m_Height;

	Filtering m_Filtering;
	Repeating m_Repeating;
	Format m_Format;
public:
	Texture(unsigned char* source, int width, int height, Filtering filtering, Repeating repeating, Format format);
	~Texture();

	void Bind(uint8_t id = 0);

	uint32_t GetWidth();
	uint32_t GetHeight();

	Filtering GetFiltering();
	Repeating GetRepeating();
	Format GetFormat();
};