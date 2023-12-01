#pragma once

#include <map>

#include <Guid.hpp>

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

class RenderResources {
private:
	std::vector<xg::Guid> m_ResourceKeys;
	std::vector<std::unique_ptr<RenderResource>> m_ResourceValues;

	std::vector<xg::Guid> m_ShaderKeys;
	std::vector<std::shared_ptr<Shader>> m_ShaderValues;

	std::map<xg::Guid, std::unique_ptr<RenderResource>> m_ResourceMap;
public:
	~RenderResources();

	xg::Guid& createCanvas(int width, int height);
	void deleteCanvas(xg::Guid canvasId);

	xg::Guid& createShader(const char* vertex, const char* fragment);
	void deleteShader(xg::Guid shaderId);

	RenderCanvasResource* getCanvas(xg::Guid canvasId);
	RenderShaderResource* getShader(xg::Guid shaderId);
};