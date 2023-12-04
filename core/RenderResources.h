#pragma once

#include <map>

#include <Guid.hpp>
#include <glm/glm.hpp>

class Shader;

struct RenderResource {
	int resourceType;
	xg::Guid guid;
};

struct RenderCanvasResource : public RenderResource {
	uint32_t id;
	uint32_t fbo;

	int width;
	int height;
};

struct RenderShaderResource : public RenderResource {
	std::shared_ptr<Shader> shader;
};

struct RenderTextureResource : public RenderResource {
	uint32_t id;

	int width;
	int height;
};

struct RenderFontGlyph {
	glm::vec2 uv1;
	glm::vec2 uv2;

	int advance;
	int width;
	int height;

	int offsetX;
	int offsetY;
};

struct RenderFontResource : public RenderResource {
	uint32_t id;

	int width;
	int height;
};

class RenderResources {
private:
	std::vector<xg::Guid> m_ResourceKeys;
	std::vector<std::unique_ptr<RenderResource>> m_ResourceValues;

	std::map<xg::Guid, std::unique_ptr<RenderResource>> m_ResourceMap;
public:
	~RenderResources();

	xg::Guid& createCanvas(int width, int height);
	void deleteCanvas(xg::Guid canvasId);

	xg::Guid& createShader(const char* vertex, const char* fragment);
	void deleteShader(xg::Guid shaderId);

	xg::Guid& createTexture(const char* path);
	void deleteTexture(xg::Guid textureId);

	RenderCanvasResource* getCanvas(xg::Guid canvasId);
	RenderShaderResource* getShader(xg::Guid shaderId);
	RenderTextureResource* getTexture(xg::Guid textureId);
};