#pragma once

struct lua_State;
class Window;
class Renderer;
class RenderResources;
class CommandList;
class DebugDrawer;

class LuaHandler {
private:
	lua_State* m_State;
public:
	LuaHandler(const char* def);
	~LuaHandler();
	
	void bindWindow(Window* window);
	void bindRenderer(Renderer* renderer);
	void bindRenderResources(RenderResources* rres);
	void bindCommandList(CommandList* cmds);
	void bindDebugDrawer(DebugDrawer* ddraw);

	void bindGlobal();

	void open(const char* file);
	void run(const char* src);
	void call(const char* func);
};