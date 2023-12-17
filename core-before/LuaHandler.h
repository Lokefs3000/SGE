#pragma once

struct lua_State;
class Window;
class Renderer;
class RenderResources;
class CommandList;
class DebugDrawer;
class SGELoader;
class TimeHandler;

class LuaHandler {
private:
	lua_State* m_State;

	void registerMetaTables();
public:
	LuaHandler(const char* def);
	~LuaHandler();
	
	void bindWindow(Window* window);
	void bindRenderer(Renderer* renderer);
	void bindRenderResources(RenderResources* rres);
	void bindDebugDrawer(DebugDrawer* ddraw);
	void bindSGELoader(SGELoader* loader);
	void bindTimeHandler(TimeHandler* time);

	void bindGlobal();

	void open(const char* file);
	void run(const char* src);
	void call(const char* func);
};