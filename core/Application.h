#pragma once

#include "globals.h"

#include <memory>
#include <string>

class DataElement;
class Window;
class EventHandler;
class Renderer;
class AssetHandler;
class LuaHandler;

class Application {
private:
	std::string m_Base;

	std::string m_LoadFunc;
	std::string m_UpdateFunc;
	std::string m_RenderFunc;
	std::string m_ExitFunc;
	bool m_UpdateThreaded;

	std::shared_ptr<DataElement> m_Configuration;
	std::shared_ptr<Window> m_Window;
	std::shared_ptr<Renderer> m_Renderer;
	std::shared_ptr<EventHandler> m_EventHandler;
	std::shared_ptr<AssetHandler> m_AssetHandler;
	std::shared_ptr<LuaHandler> m_LuaHandler;
public:
	EXPORT int Run(char* argv[], int argc);

	int RunSetup(char* argv[], int argc);
	int Initialize();
	int LoopLogic();
	int Shutdown();
};