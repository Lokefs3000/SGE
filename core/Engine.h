#pragma once

#include <memory>

#include "exports.h"

class Window;
class EventHandler;
class Renderer;
class LuaHandler;
class RenderResources;
class DebugDrawer;

class Engine {
private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<EventHandler> m_EventHandler;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<LuaHandler> m_LuaHandler;
	std::unique_ptr<RenderResources> m_RenderResources;
	std::unique_ptr<DebugDrawer> m_DebugDrawer;
public:
	SGE Engine(int argc, char* argv[]);
	SGE ~Engine();

	SGE int Run();
};