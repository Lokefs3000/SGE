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
#include "DebugDrawer.h"
#include "SGELoader.h"
#include "TimeHandler.h"

#include "resdata/res_lua_boot_lua.h"

#pragma region FunctionVariables
static Window* s_Window = NULL;
static Renderer* s_Renderer = NULL;
static RenderResources* s_RenderResources = NULL;
static DebugDrawer* s_DebugDrawer = NULL;
static SGELoader* s_SGELoader = NULL;
static TimeHandler* s_TimeHandler = NULL;
static std::string s_DefPath = "";
#pragma endregion

#pragma region FunctionInitials
static int LuaHandler_defaultEmpty(lua_State* state);

static int Window_setTitle(lua_State* state);
static int Window_setWidth(lua_State* state);
static int Window_setHeight(lua_State* state);

static int Renderer_clear(lua_State* state);
static int Renderer_drawRect(lua_State* state);
static int Renderer_draw(lua_State* state);

static int RenderResources_createCanvas(lua_State* state);
static int RenderResources_deleteCanvas(lua_State* state);
static int RenderResources_getCanvas(lua_State* state);
static int RenderResources_createShader(lua_State* state);
static int RenderResources_deleteShader(lua_State* state);
static int RenderResources_getShader(lua_State* state);
static int RenderResources_createTexture(lua_State* state);
static int RenderResources_deleteTexture(lua_State* state);
static int RenderResources_getTexture(lua_State* state);

static int DebugDrawer_text(lua_State* state);

static int SGELoader_createSGE(lua_State* state);
static int SGELoader_deleteSGE(lua_State* state);

static int TimeHandler_retimeDelta(lua_State* state);
static int TimeHandler_delta(lua_State* state);
#pragma endregion

#pragma region MetaTableInitials
static int vec4reg_gc(lua_State* state);
static int vec4reg_index(lua_State* state);
static int vec4reg_newindex(lua_State* state);
static int vec4reg_new(lua_State* state);
#pragma endregion

#pragma region MetaTableData
static const struct luaL_Reg vec4_meta[] = {
	{"__gc", vec4reg_gc},
	{"__index", vec4reg_index},
	{"__newindex", vec4reg_newindex},
	{NULL, NULL}
};

static const struct luaL_Reg vec4_method[] = {
	{"new", vec4reg_new},
	{NULL, NULL}
};
#pragma endregion

void LuaHandler::registerMetaTables()
{
	int lib_id, meta_id;
	
	lua_createtable(m_State, 0, 0);
	lib_id = lua_gettop(m_State);
	luaL_newmetatable(m_State, "Vector4");
	meta_id = lua_gettop(m_State);
	luaL_setfuncs(m_State, vec4_meta, 0);
	luaL_newlib(m_State, vec4_method);
	lua_setfield(m_State, meta_id, "__index");
	luaL_newlib(m_State, vec4_meta);
	lua_setfield(m_State, meta_id, "__metatable");
	lua_setmetatable(m_State, lib_id);
	lua_setglobal(m_State, "Vector4");
}

LuaHandler::LuaHandler(const char* def)
{
	m_State = luaL_newstate();
	luaL_openlibs(m_State);

	registerMetaTables();

	lua_newtable(m_State);

	lua_pushstring(m_State, def);
	lua_setfield(m_State, -2, "def");
	s_DefPath = def;
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

	lua_pushcfunction(m_State, Renderer_clear);
	lua_setfield(m_State, -2, "clear");
	lua_pushcfunction(m_State, Renderer_drawRect);
	lua_setfield(m_State, -2, "drawRect");
	lua_pushcfunction(m_State, Renderer_draw);
	lua_setfield(m_State, -2, "draw");

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

	lua_pushcfunction(m_State, RenderResources_createTexture);
	lua_setfield(m_State, -2, "createTexture");
	lua_pushcfunction(m_State, RenderResources_deleteTexture);
	lua_setfield(m_State, -2, "deleteTexture");
	lua_pushcfunction(m_State, RenderResources_getTexture);
	lua_setfield(m_State, -2, "getTexture");

	lua_setfield(m_State, -2, "resources");
}

void LuaHandler::bindDebugDrawer(DebugDrawer* ddraw)
{
	s_DebugDrawer = ddraw;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, DebugDrawer_text);
	lua_setfield(m_State, -2, "text");

	lua_setfield(m_State, -2, "debug");
}

void LuaHandler::bindSGELoader(SGELoader* loader)
{
	s_SGELoader = loader;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, SGELoader_createSGE);
	lua_setfield(m_State, -2, "createSGE");
	lua_pushcfunction(m_State, SGELoader_deleteSGE);
	lua_setfield(m_State, -2, "deleteSGE");

	lua_setfield(m_State, -2, "loader");
}

void LuaHandler::bindTimeHandler(TimeHandler* time)
{
	s_TimeHandler = time;

	lua_newtable(m_State);

	lua_pushcfunction(m_State, TimeHandler_retimeDelta);
	lua_setfield(m_State, -2, "retimeDelta");
	lua_pushcfunction(m_State, TimeHandler_delta);
	lua_setfield(m_State, -2, "delta");

	lua_setfield(m_State, -2, "time");
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

#pragma region MetaTableBindings
int vec4reg_gc(lua_State* state)
{
	return 0;
}

int vec4reg_index(lua_State* state)
{
	return 0;
}

int vec4reg_newindex(lua_State* state)
{
	return 0;
}

int vec4reg_new(lua_State* state)
{
	double x = luaL_checknumber(state, 1);
	double y = luaL_checknumber(state, 2);
	double z = luaL_checknumber(state, 3);
	double w = luaL_checknumber(state, 4);
	glm::vec4& v4 = *(glm::vec4*)lua_newuserdata(state, sizeof(glm::vec4));
	v4.x = x; v4.y = y; v4.z = z; v4.w = w;
	luaL_getmetatable(state, "Vector4");
	lua_setmetatable(state, -2);
	return 1;
}
#pragma endregion

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

int Renderer_clear(lua_State* state)
{
	ASSERT(s_Renderer != NULL, "s_Renderer is nullptr!", 0);
	glm::vec4& color = *(glm::vec4*)luaL_checkudata(state, 1, "Vector4");
	s_Renderer->clear(color);
	return 0;
}

int Renderer_drawRect(lua_State* state)
{
	ASSERT(s_Renderer != NULL, "s_Renderer is nullptr!", 0);
	glm::vec4& color = *(glm::vec4*)luaL_checkudata(state, 1, "Vector4");
	lua_Integer x = luaL_checkinteger(state, 2);
	lua_Integer y = luaL_checkinteger(state, 3);
	lua_Integer w = luaL_checkinteger(state, 4);
	lua_Integer h = luaL_checkinteger(state, 5);
	s_Renderer->drawRect(color, x, y, w, h);
	return 0;
}

int Renderer_draw(lua_State* state)
{
	ASSERT(s_Renderer != NULL, "s_Renderer is nullptr!", 0);
	s_Renderer->draw();
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

int RenderResources_createTexture(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	const char* path = luaL_checkstring(state, 1);
	std::string spath = s_DefPath + path;
	xg::Guid& guid = s_RenderResources->createTexture(spath.c_str());
	lua_pushlightuserdata(state, &guid);
	return 1;
}

int RenderResources_deleteTexture(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	if (!lua_islightuserdata(state, 1))
		return 0;
	void* guid = lua_touserdata(state, 1);
	s_RenderResources->deleteTexture(*(xg::Guid*)guid);
	return 0;
}

int RenderResources_getTexture(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_RenderResources is nullptr!", 0);
	ASSERT(lua_islightuserdata(state, 1), "RenderResources_getShader did not recieve lightuserdata!", 0);
	void* guid = lua_touserdata(state, 1);
	RenderTextureResource* res = s_RenderResources->getTexture(*(xg::Guid*)guid);
	lua_pushlightuserdata(state, &res);
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

int SGELoader_createSGE(lua_State* state)
{
	ASSERT(s_SGELoader != NULL, "s_SGELoader is nullptr!", 0);
	const char* file = luaL_checkstring(state, 1);
	sgeElement* res = s_SGELoader->createSGE(file);
	lua_pushlightuserdata(state, &res);
	return 0;
}

int SGELoader_deleteSGE(lua_State* state)
{
	ASSERT(s_RenderResources != NULL, "s_SGELoader is nullptr!", 0);
	if (!lua_islightuserdata(state, 1))
		return 0;
	void* sge = lua_touserdata(state, 1);
	s_SGELoader->deleteSGE((sgeElement*)sge);
	return 0;
}

int TimeHandler_retimeDelta(lua_State* state)
{
	ASSERT(s_TimeHandler != NULL, "s_TimeHandler is nullptr!", 0);
	s_TimeHandler->retimeDelta();
	return 0;
}

int TimeHandler_delta(lua_State* state)
{
	ASSERT(s_TimeHandler != NULL, "s_TimeHandler is nullptr!", 0);
	double delta = s_TimeHandler->delta();
	lua_pushnumber(state, delta);
	return 1;
}
#pragma endregion
