#pragma once

#include <memory>
#include <string>

#include "exports.h"

class Window;
class EventHandler;
class Renderer;
class LuaHandler;
class RenderResources;
class DebugDrawer;
class SGELoader;
class TimeHandler;

class Engine {
private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<EventHandler> m_EventHandler;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<LuaHandler> m_LuaHandler;
	std::unique_ptr<RenderResources> m_RenderResources;
	std::unique_ptr<DebugDrawer> m_DebugDrawer;
	std::unique_ptr<SGELoader> m_SGELoader;
	std::unique_ptr<TimeHandler> m_TimeHandler;
	std::string m_DefPath;
public:
	SGE Engine(int argc, char* argv[]);
	SGE ~Engine();

	SGE int Run();
};