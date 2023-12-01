#pragma once

#include <memory>
#include <functional>

#include <Guid.hpp>

struct SDL_Window;
class CommandList;
struct Command;
class RenderResources;

class Renderer {
private:
	void* m_OpenGL;
	RenderResources* m_Resources;

	std::unique_ptr<CommandList> m_CommandList;
	std::function<void(Command*)> m_DrawFunctions[3];

	xg::Guid m_LastShader = DEAD_GUID;

	void clearCmd(Command* cmd);
	void canvasCmd(Command* cmd);
	void shaderCmd(Command* cmd);
public:
	const xg::Guid DEAD_GUID = xg::Guid("000000000000000");
	const xg::Guid AUTO_GUID = xg::Guid("fffffffffffffff");

	Renderer(SDL_Window* window, RenderResources* res);
	~Renderer();

	CommandList* beginCmd();
	void drawCmd();
	void flushCmd();

	CommandList* getCmds();
};