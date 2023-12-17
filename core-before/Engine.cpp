#include "pch.h"
#include "Engine.h"

#include "Window.h"
#include "EventHandler.h"
#include "Renderer.h"
#include "LuaHandler.h"
#include "RenderResources.h"
#include "DebugDrawer.h"
#include "SGELoader.h"
#include "TimeHandler.h"

Engine::Engine(int argc, char* argv[])
{
    const char* def = "";
    bool forceDebug = false;

#ifdef _DEBUG
    forceDebug = true;
#endif


    for (size_t i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-debug") {
            forceDebug = true;
            continue;
        }
        else if (arg == "-ndebug") {
            forceDebug = false;
            continue;
        }
        def = argv[i];
    }
    m_DefPath = def;

    m_Window = std::make_unique<Window>();
    m_EventHandler = std::make_unique<EventHandler>();
    m_RenderResources = std::make_unique<RenderResources>();
    m_Renderer = std::make_unique<Renderer>(m_Window.get(), m_RenderResources.get(), forceDebug);
    m_LuaHandler = std::make_unique<LuaHandler>(def);
    m_DebugDrawer = std::make_unique<DebugDrawer>();
    m_SGELoader = std::make_unique<SGELoader>(def);
    m_TimeHandler = std::make_unique<TimeHandler>();

    m_LuaHandler->bindWindow(m_Window.get());
    m_LuaHandler->bindRenderer(m_Renderer.get());
    m_LuaHandler->bindRenderResources(m_RenderResources.get());
    m_LuaHandler->bindDebugDrawer(m_DebugDrawer.get());
    m_LuaHandler->bindSGELoader(m_SGELoader.get());
    m_LuaHandler->bindTimeHandler(m_TimeHandler.get());
    m_LuaHandler->bindGlobal();

    m_EventHandler->bind(EVENT_HANDLER_BIND1(Window::eventProc, m_Window.get()));
}

Engine::~Engine()
{
    m_LuaHandler.reset();
    m_DebugDrawer.reset();
    m_Renderer.reset();
    m_RenderResources.reset();
    m_Window.reset();
    m_EventHandler.reset();
    m_TimeHandler.reset();
}

int Engine::Run()
{
    std::string path = m_DefPath + "main.lua";
    m_LuaHandler->open(path.c_str());
    m_LuaHandler->call("sge_load");

    while (!m_Window->isClosed())
    {
        m_EventHandler->process();

        m_LuaHandler->call("sge_update");
        m_LuaHandler->call("sge_render");

        m_DebugDrawer->draw();
        m_Window->swapWindow();

        m_TimeHandler->retimeDelta();
    }

    m_LuaHandler->call("sge_exit");

    return 0;
}
