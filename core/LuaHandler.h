#pragma once

#include <string>
#include <memory>
#include <type_traits>
#include <tuple>
#include <iostream>

struct lua_State;

class AssetHandler;
class DataElement;
class Renderer;

class LuaHandler {
private:
	lua_State* m_State;

	void StartTable();
	void PushEnum(std::string name, int value);
	void PushTable(std::string name);
public:
	LuaHandler(std::weak_ptr<DataElement> config, std::weak_ptr<AssetHandler> assetHandler, std::weak_ptr<Renderer> renderer);
	~LuaHandler();

	void DumpStack();

	void Call(std::string& source);
	void Invoke(std::string function);
};