#include "pch.h"
#include "LuaHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include "lua/luajit.h"
}

#include "exports.h"

#include "Window.h"
#include "Renderer.h"
#include "RenderResources.h"
#include "DebugLogger.h"
#include "CommandList.h"
#include "DebugDrawer.h"

#include "resdata/res_lua_boot_lua.h"

#pragma region FunctionVariables
static Window* s_Window = NULL;
static Renderer* s_Renderer = NULL;
static RenderResources* s_RenderResources = NULL;
static CommandList* s_CommandList = NULL;
static DebugDrawer* s_DebugDrawer = NULL;
#pragma endregion

#pragma region FunctionInitials
static int LuaHandler_defaultEmpty(lua_State* state);

static int Window_setTitle(lua_State* state);
static int Window_setWidth(lua_State* state);
static int Window_setHeight(lua_State* state);

static int Renderer_beginCmd(lua_State* state);
static int Renderer_drawCmd(lua_State* state);
static int Renderer_flushCmd(lua_State* state);

static int RenderResources_createCanvas(lua_State* state);
static int RenderResources_deleteCanvas(lua_State* state);
static int RenderResources_getCanvas(lua_State* state);
static int RenderResources_createShader(lua_State* state);
static int RenderResources_deleteShader(lua_State* state);
static int RenderResources_getShader(lua_State* state);

static int CommandList_clear(lua_State* state);
static int CommandList_canvas(lua_State* state);

static int DebugDrawer_text(lua_State* state);
#pragma endregion

LuaHandler::LuaHandler(const char* def)
{
	m_State = luaL_newstate();
	luaL_openlibs(m_State);

	lua_newtable(m_State);

	lua_pushstring(m_State, def);
	lua_setfield(m_State, -2, "def");
}

LuaHandler::~LuaHandler()
{
	lua_close(m_State);
}

void LuaHandler::bindWindow(Window* window)
{
	s_Window = window;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, Window_setTitle);
	lua_setfield(m_State, -2, "setTitle");
	lua_pushcfunction(m_State, Window_setWidth);
	lua_setfield(m_State, -2, "setWidth");
	lua_pushcfunction(m_State, Window_setHeight);
	lua_setfield(m_State, -2, "setHeight");

	lua_setfield(m_State, -2, "window");
}

void LuaHandler::bindRenderer(Renderer* renderer)
{
	s_Renderer = renderer;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, Renderer_beginCmd);
	lua_setfield(m_State, -2, "beginCmd");
	lua_pushcfunction(m_State, Renderer_drawCmd);
	lua_setfield(m_State, -2, "drawCmd");
	lua_pushcfunction(m_State, Renderer_flushCmd);
	lua_setfield(m_State, -2, "flushCmd");

	lua_setfield(m_State, -2, "renderer");
}

void LuaHandler::bindRenderResources(RenderResources* rres)
{
	s_RenderResources = rres;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, RenderResources_createCanvas);
	lua_setfield(m_State, -2, "createCanvas");
	lua_pushcfunction(m_State, RenderResources_deleteCanvas);
	lua_setfield(m_State, -2, "deleteCanvas");
	lua_pushcfunction(m_State, RenderResources_getCanvas);
	lua_setfield(m_State, -2, "getCanvas");

	lua_pushcfunction(m_State, RenderResources_createShader);
	lua_setfield(m_State, -2, "createShader");
	lua_pushcfunction(m_State, RenderResources_deleteShader);
	lua_setfield(m_State, -2, "deleteShader");
	lua_pushcfunction(m_State, RenderResources_getShader);
	lua_setfield(m_State, -2, "getShader");

	lua_setfield(m_State, -2, "resources");
}

void LuaHandler::bindCommandList(CommandList* cmds)
{
	s_CommandList = cmds;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, CommandList_clear);
	lua_setfield(m_State, -2, "clear");
	lua_pushcfunction(m_State, CommandList_canvas);
	lua_setfield(m_State, -2, "canvas");

	lua_setfield(m_State, -2, "commands");
}

void LuaHandler::bindDebugDrawer(DebugDrawer* ddraw)
{
	s_DebugDrawer = ddraw;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, DebugDrawer_text);
	lua_setfield(m_State, -2, "text");

	lua_setfield(m_State, -2, "debug");
}

void LuaHandler::bindGlobal()
{
	lua_pushcfunction(m_State, LuaHandler_defaultEmpty);
	lua_setfield(m_State, -2, "load");
	lua_pushcfunction(m_State, LuaHandler_defaultEmpty);
	lua_setfield(m_State, -2, "update");
	lua_pushcfunction(m_State, LuaHandler_defaultEmpty);
	lua_setfield(m_State, -2, "render");

	lua_setglobal(m_State, "sge");

	run(res_lua_boot_lua);
}

void LuaHandler::open(const char* file)
{
	int ret = luaL_dofile(m_State, file);
	if (ret != 0) {
		std::cout << "Lua error occured! Message: " << lua_tostring(m_State, -1) << std::endl;
	}
}

void LuaHandler::run(const char* src)
{
	int ret = luaL_dostring(m_State, src);
	if (ret != 0) {
		std::cout << "Lua error occured! Message: " << lua_tostring(m_State, -1) << std::endl;
	}
}

void LuaHandler::call(const char* func)
{
	lua_getglobal(m_State, func);
	if (lua_isfunction(m_State, -1)) {
		int ret = lua_pcall(m_State, 0, 0, 0);
		if (ret != 0) {
			std::cout << "Lua error occured! Message: " << lua_tostring(m_State, -1) << std::endl;
		}
	}
	else
		std::cout << func << " is not a global lua function!" << std::endl;
}

#pragma region FunctionBindings
int LuaHandler_defaultEmpty(lua_State* state)
{
	return 0;
}

int Window_setTitle(lua_State* state)
{
	ASSERT(s_Window != NULL, "s_Window is nullptr!", 0);
	const char* title = luaL_checkstring(state, 1);
	s_Window->setTitle((char*)title);
	return 0;
}

int Window_setWidth(lua_State* state)
{
	ASSERT(s_Window != NULL, "s_Window is nullptr!", 0);
	lua_Integer width = luaL_checkinteger(state, 1);
	s_Window->setWidth(width);
	return 0;
}

int Window_setHeight(lua_State* state)
{
	ASSERT(s_Window != NULL, "s_Window is nullptr!", 0);
	lua_Integer height = luaL_checkinteger(state, 1);
	s_Window->setHeight(height);
	return 0;
}

int Renderer_beginCmd(lua_State* state)
{
	ASSERT(s_Renderer != NULL, "s_Renderer is nullptr!", 0);
	CommandList* cmd = s_Renderer->beginCmd();
	lua_pushlightuserdata(state, cmd);
	return 1;
}

int Renderer_drawCmd(lua_State* state)
{
	ASSERT(s_Renderer != NULL, "s_Renderer is nullptr!", 0);
	s_Renderer->drawCmd();
	return 0;
}

int Renderer_flushCmd(lua_State* state)
{
	ASSERT(s_Renderer != NULL, "s_Renderer is nullptr!", 0);
	s_Renderer->flushCmd();
	return 0;
}

int RenderResources_createCanvas(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	int width = luaL_checkint(state, 1);
	int height = luaL_checkint(state, 2);
	xg::Guid& guid = s_RenderResources->createCanvas(width, height);
	lua_pushlightuserdata(state, &guid); 
	return 1;
}

int RenderResources_deleteCanvas(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	if (!lua_islightuserdata(state, 1))
		return 0;
	void* guid = lua_touserdata(state, 1);
	s_RenderResources->deleteCanvas(*(xg::Guid*)guid);
	return 0;
}

int RenderResources_getCanvas(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	ASSERT(lua_islightuserdata(state, 1), "RenderResources_getCanvas did not recieve lightuserdata!", 0);
	void* guid = lua_touserdata(state, 1);
	RenderCanvasResource* res = s_RenderResources->getCanvas(*(xg::Guid*)guid);
	lua_pushlightuserdata(state, &res);
	return 0;
}

int RenderResources_createShader(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	const char* vertex = luaL_checkstring(state, 1);
	const char* fragment = luaL_checkstring(state, 2);
	xg::Guid& guid = s_RenderResources->createShader(vertex, fragment);
	lua_pushlightuserdata(state, &guid);
	return 1;
}

int RenderResources_deleteShader(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	if (!lua_islightuserdata(state, 1))
		return 0;
	void* guid = lua_touserdata(state, 1);
	s_RenderResources->deleteShader(*(xg::Guid*)guid);
	return 0;
}

int RenderResources_getShader(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	ASSERT(lua_islightuserdata(state, 1), "RenderResources_getShader did not recieve lightuserdata!", 0);
	void* guid = lua_touserdata(state, 1);
	RenderShaderResource* res = s_RenderResources->getShader(*(xg::Guid*)guid);
	lua_pushlightuserdata(state, &res);
	return 0;
}

int CommandList_clear(lua_State* state)
{
	ASSERT(s_CommandList != NULL, "s_CommandList is nullptr!", 0);
	double r = luaL_checknumber(state, 1);
	double g = luaL_checknumber(state, 2);
	double b = luaL_checknumber(state, 3);
	double a = luaL_checknumber(state, 4);
	s_CommandList->clear(r, g, b, a);
	return 0;
}

int CommandList_canvas(lua_State* state)
{
	ASSERT(s_CommandList != NULL, "s_CommandList is nullptr!", 0);
	ASSERT(lua_islightuserdata(state, 1), "RenderResources_getCanvas did not recieve lightuserdata!", 0);
	void* guid = lua_touserdata(state, 1);
	s_CommandList->canvas(*(xg::Guid*)guid);
	return 0;
}

int DebugDrawer_text(lua_State* state)
{
	ASSERT(s_DebugDrawer != NULL, "s_DebugDrawer is nullptr!", 0);
	const char* text = luaL_checkstring(state, 1);
	lua_Integer color = luaL_checkinteger(state, 2);
	lua_Integer x = luaL_checkinteger(state, 3);
	lua_Integer y = luaL_checkinteger(state, 4);
	s_DebugDrawer->text(text, color, x, y);
	return 0;
}
#pragma endregion
