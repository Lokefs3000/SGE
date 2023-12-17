#include "pch.h"
#include "Application.h"

#include "DataParser.h"

#include "Window.h"
#include "Renderer.h"
#include "EventHandler.h"
#include "AssetHandler.h"
#include "LuaHandler.h"
#include "DebugMessenger.h"

#include "resdata/res_defaultConfig_sge.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void emscripten_main_loop_app_func(void* arg) { //disgust :(
    if (!((Application*)arg)->LoopLogic()) {
        emscripten_cancel_main_loop();
    }
}
#endif

int Application::Run(char* argv[], int argc)
{
    RunSetup(argv, argc);
    Initialize();

    std::string src = m_AssetHandler->GetSource("main.lua");
    m_LuaHandler->Call(src);
    m_LuaHandler->Invoke(m_LoadFunc);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(&emscripten_main_loop_app_func, this, 0, true);
#else
    while (LoopLogic());
#endif
    m_LuaHandler->Invoke(m_ExitFunc);

    Shutdown();

    return 0;
}

int Application::RunSetup(char* argv[], int argc)
{
    if (argc > 1)
        m_Base = argv[1];

    return 0;
}

int Application::Initialize()
{
    m_AssetHandler = std::make_shared<AssetHandler>(m_Base);
    std::string config = m_AssetHandler->GetSource("config.sge");
    if (config.size() == 0) {
        DebugError("Failed to retrieve application config! Reverting to built-in!.");
        config = res_defaultConfig_sge;
    }
    m_Configuration = DataParser::ParseSourceData(config);

    auto calls = m_Configuration->GetChild("config")->GetChild("calls");
    m_LoadFunc = calls->GetValue("loadfunc").ValString;
    m_ExitFunc = calls->GetValue("exitfunc").ValString;
    m_UpdateFunc = calls->GetValue("updatefunc").ValString;
    m_RenderFunc = calls->GetValue("renderfunc").ValString;
    m_UpdateThreaded = calls->GetChild("updateThread")->GetValue("enabled").ValInt;

    m_Window = std::make_shared<Window>(m_Configuration);
    m_Renderer = std::make_shared<Renderer>(m_Window, m_Configuration);
    m_EventHandler = std::make_shared<EventHandler>();
    m_LuaHandler = std::make_shared<LuaHandler>(m_Configuration, m_AssetHandler, m_Renderer);
    return 0;
}

int Application::LoopLogic()
{
    m_EventHandler->HandleEvents();

    if (!m_UpdateThreaded) {
        m_LuaHandler->Invoke(m_UpdateFunc);
    }

    m_Renderer->BeginFrame();
    m_LuaHandler->Invoke(m_RenderFunc);
    m_Renderer->EndFrame();
    m_Window->PerformBufferSwap();
    return true;
}

int Application::Shutdown()
{
    return 0;
}
