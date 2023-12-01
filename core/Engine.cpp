#include "pch.h"
#include "Engine.h"

#include "Window.h"
#include "EventHandler.h"
#include "Renderer.h"
#include "CommandList.h"
#include "LuaHandler.h"
#include "RenderResources.h"
#include "DebugDrawer.h"

Engine::Engine(int argc, char* argv[])
{
    const char* def = "";

    for (size_t i = 1; i < argc; i++)
    {
        def = argv[i];
    }

    m_Window = std::make_unique<Window>();
    m_EventHandler = std::make_unique<EventHandler>();
    m_RenderResources = std::make_unique<RenderResources>();
    m_Renderer = std::make_unique<Renderer>(m_Window->getWindow(), m_RenderResources.get());
    m_LuaHandler = std::make_unique<LuaHandler>(def);
    m_DebugDrawer = std::make_unique<DebugDrawer>();

    m_LuaHandler->bindWindow(m_Window.get());
    m_LuaHandler->bindRenderer(m_Renderer.get());
    m_LuaHandler->bindRenderResources(m_RenderResources.get());
    m_LuaHandler->bindCommandList(m_Renderer->getCmds());
    m_LuaHandler->bindDebugDrawer(m_DebugDrawer.get());
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
}

int Engine::Run()
{
    m_LuaHandler->open("C:\\Users\\Loke\\source\\repos\\SGE\\example-app\\main.lua");
    m_LuaHandler->call("sge_load");

    while (!m_Window->isClosed())
    {
        m_EventHandler->process();

        m_LuaHandler->call("sge_update");

        CommandList* cmds = m_Renderer->beginCmd();

        m_LuaHandler->call("sge_render");

        m_Renderer->drawCmd();
        m_DebugDrawer->draw();
        m_Window->swapWindow();
    }

    m_LuaHandler->call("sge_exit");

    return 0;
}
