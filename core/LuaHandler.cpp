#include "pch.h"
#include "LuaHandler.h"

#if defined(LUA_USE52) || defined(__EMSCRIPTEN__)
#include <Lua52/lua.hpp>
#pragma comment (lib, "lua52.lib")
#define LUA52
#else
#include <LuaJIT/lua.hpp>
#pragma comment (lib, "lua51.lib")
#pragma comment (lib, "luajit.lib")
#define LUAJIT
#endif


#include "DebugMessenger.h"

#include "AssetHandler.h"
#include "Renderer.h"
#include "Texture.h"

void LuaHandler::StartTable()
{
	lua_newtable(m_State);
}

void LuaHandler::PushEnum(std::string name, int value)
{
	lua_pushnumber(m_State, value);
	lua_setfield(m_State, -2, name.c_str());
}

void LuaHandler::PushTable(std::string name)
{
	lua_setglobal(m_State, name.c_str());
}

int GetField(lua_State* L, std::string key)
{
	int result;
	lua_pushstring(L, key.c_str());
	lua_gettable(L, -2);
	if (!lua_isnumber(L, -1))
		return -1;
	result = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

int LuaErrors(lua_State* L) { //Directly from "lua.c" lmao
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
#ifdef LUA52
	lua_getglobal(L, "debug");
#else
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
#endif
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

AssetHandler* s_AssetHandler;
LuaHandler* s_LuaHandler;
Renderer* s_Renderer;

int l_tex_new(lua_State* L);
int l_tex__gc(lua_State* L);

int l_vec2_new(lua_State* L);
int l_vec2__gc(lua_State* L);
int l_vec3_new(lua_State* L);
int l_vec3__gc(lua_State* L);
int l_vec4_new(lua_State* L);
int l_vec4__gc(lua_State* L);

int l_rnd_drawSprite(lua_State* L);

LuaHandler::LuaHandler(std::weak_ptr<DataElement> config, std::weak_ptr<AssetHandler> assetHandler, std::weak_ptr<Renderer> renderer)
{
	s_AssetHandler = assetHandler.lock().get();
	s_Renderer = renderer.lock().get();
	s_LuaHandler = this;

	m_State = luaL_newstate();
	luaL_openlibs(m_State);

	////////////// ENUMS //////////////

	{ // Enum:Filtering //
		StartTable();
		PushEnum("BiLinear", (int)Filtering::BiLinear);
		PushEnum("Nearest", (int)Filtering::Nearest);
		PushTable("Filtering");
	}
	{ // Enum:Repeating //
		StartTable();
		PushEnum("Clamp", (int)Repeating::Clamp);
		PushEnum("Repeat", (int)Repeating::Repeat);
		PushTable("Repeating");
	}

	////////////// META TABLES //////////////

	{ // Meta:Texture
		luaL_Reg registry[] = {
			{ "new", l_tex_new },
			{ "__gc", l_tex__gc },
			{ NULL, NULL }
		};

		luaL_newmetatable(m_State, "sge_Texture");
		luaL_setfuncs(m_State, registry, 0);
		lua_pushvalue(m_State, -1);
		lua_setfield(m_State, -1, "__index");
		lua_setglobal(m_State, "Texture");
	}

	{ // Meta:Vector2
		luaL_Reg registry[] = {
			{ "new", l_vec2_new },
			{ "__gc", l_vec2__gc },
			{ NULL, NULL }
		};

		luaL_newmetatable(m_State, "sge_Vector2");
		luaL_setfuncs(m_State, registry, 0);
		lua_pushvalue(m_State, -1);
		lua_setfield(m_State, -1, "__index");
		lua_setglobal(m_State, "Vector2");
	}

	{ // Meta:Vector3
		luaL_Reg registry[] = {
			{ "new", l_vec3_new },
			{ "__gc", l_vec3__gc },
			{ NULL, NULL }
		};

		luaL_newmetatable(m_State, "sge_Vector3");
		luaL_setfuncs(m_State, registry, 0);
		lua_pushvalue(m_State, -1);
		lua_setfield(m_State, -1, "__index");
		lua_setglobal(m_State, "Vector3");
	}

	{ // Meta:Vector4
		luaL_Reg registry[] = {
			{ "new", l_vec4_new },
			{ "__gc", l_vec4__gc },
			{ NULL, NULL }
		};

		luaL_newmetatable(m_State, "sge_Vector4");
		luaL_setfuncs(m_State, registry, 0);
		lua_pushvalue(m_State, -1);
		lua_setfield(m_State, -1, "__index");
		lua_setglobal(m_State, "Vector4");
	}

	////////////// FUNCTIONS //////////////

	{ //Func:Renderer
		lua_newtable(m_State);

		lua_pushcfunction(m_State, l_rnd_drawSprite);
		lua_setfield(m_State, -2, "DrawSprite");

		lua_setglobal(m_State, "Renderer");
	}

	/*m_State = sol::state();
	m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math, sol::lib::table, sol::lib::bit32, sol::lib::utf8);
	
	m_State.new_enum("Filtering", "BiLinear", Filtering::BiLinear, "Nearest", Filtering::Nearest);
	m_State.new_enum("Repeating", "Clamp", Repeating::Clamp, "Repeat", Repeating::Repeat);

 	sol::usertype<lua_Texture> texture = m_State.new_usertype<lua_Texture>("Texture", sol::constructors<lua_Texture(std::string, sol::table)>());
	texture.set("Width", sol::readonly(&lua_Texture::w));
	texture.set("Height", sol::readonly(&lua_Texture::h));
	texture.set("Filtering", sol::readonly(&lua_Texture::filter));
	texture.set("Repeating", sol::readonly(&lua_Texture::repeat));
	texture.set("Format", sol::readonly(&lua_Texture::format));*/
}

LuaHandler::~LuaHandler()
{
	lua_close(m_State);
}

void LuaHandler::DumpStack()
{
	int top = lua_gettop(m_State);
	for (int i = 1; i <= top; i++) {
		DebugInfo(luaL_typename(m_State, i));
		switch (lua_type(m_State, i)) {
		case LUA_TNUMBER:
			DebugInfo(lua_tonumber(m_State, i));
			break;
		case LUA_TSTRING:
			DebugInfo(lua_tostring(m_State, i));
			break;
		case LUA_TBOOLEAN:
			DebugInfo((lua_toboolean(m_State, i) ? "true" : "false"));
			break;
		case LUA_TNIL:
			DebugInfo("nil");
			break;
		default:
			DebugInfo(lua_topointer(m_State, i));
			break;
		}
	}
}

void LuaHandler::Call(std::string& source)
{
	luaL_loadstring(m_State, source.c_str());
	if (lua_pcall(m_State, 0, 0, 0) != 0) {
		DebugWarning("Lua error occured! Error message: ", lua_tostring(m_State, -1));
	}
}

void LuaHandler::Invoke(std::string function)
{
	lua_getglobal(m_State, function.c_str());
	if (lua_isfunction(m_State, -1)) {
		if (lua_pcall(m_State, 0, 0, 0) != 0) {
			DebugWarning("Lua error occured invoking ", function, "! Error message: ", lua_tostring(m_State, -1));
		}
	}
	else {
		DebugWarning("Function: ", function, " is not a function!");
		lua_pop(m_State, 1);
	}
}

int l_tex_new(lua_State* L)
{
	const char* path = luaL_checkstring(L, 1);
	Filtering filtering = (Filtering)GetField(L, "Filter");
	Repeating repeating = (Repeating)GetField(L, "Repeat");
	Texture** udata = (Texture**)lua_newuserdata(L, sizeof(Texture*));
	*udata = s_AssetHandler->GetTextureRaw(path, filtering, repeating);
	luaL_getmetatable(L, "sge_Texture");
	lua_setmetatable(L, -2);
	return 1;
}

int l_tex__gc(lua_State* L)
{
	Texture* tex = *(Texture**)luaL_checkudata(L, 1, "sge_Texture");
	delete tex;
	return 0;
}

int l_vec2_new(lua_State* L)
{
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	glm::vec2** udata = (glm::vec2**)lua_newuserdata(L, sizeof(glm::vec2*));
	*udata = new glm::vec2(x, y);
	luaL_getmetatable(L, "sge_Vector2");
	lua_setmetatable(L, -2);
	return 1;
}

int l_vec2__gc(lua_State* L)
{
	glm::vec2* tex = *(glm::vec2**)luaL_checkudata(L, 1, "sge_Vector2");
	delete tex;
	return 0;
}

int l_vec3_new(lua_State* L)
{
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);
	glm::vec3** udata = (glm::vec3**)lua_newuserdata(L, sizeof(glm::vec3*));
	*udata = new glm::vec3(x, y, z);
	luaL_getmetatable(L, "sge_Vector3");
	lua_setmetatable(L, -2);
	return 1;
}

int l_vec3__gc(lua_State* L)
{
	glm::vec3* tex = *(glm::vec3**)luaL_checkudata(L, 1, "sge_Vector3");
	delete tex;
	return 0;
}

int l_vec4_new(lua_State* L)
{
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float z = luaL_checknumber(L, 3);
	float w = luaL_checknumber(L, 4);
	glm::vec4** udata = (glm::vec4**)lua_newuserdata(L, sizeof(glm::vec4*));
	*udata = new glm::vec4(x, y, z, w);
	luaL_getmetatable(L, "sge_Vector4");
	lua_setmetatable(L, -2);
	return 1;
}

int l_vec4__gc(lua_State* L)
{
	glm::vec4* tex = *(glm::vec4**)luaL_checkudata(L, 1, "sge_Vector4");
	delete tex;
	return 0;
}

int l_rnd_drawSprite(lua_State* L)
{
	glm::vec2* pos = *(glm::vec2**)luaL_checkudata(L, 1, "sge_Vector2");
	glm::vec2* size = *(glm::vec2**)luaL_checkudata(L, 2, "sge_Vector2");
	glm::vec4* col = *(glm::vec4**)luaL_checkudata(L, 3, "sge_Vector4");
	Texture* tex = *(Texture**)luaL_checkudata(L, 4, "sge_Texture");
	s_Renderer->DrawSprite(*pos, *size, *col, tex);
	return 0;
}
