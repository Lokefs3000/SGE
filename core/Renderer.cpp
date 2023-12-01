#include "pch.h"
#include "Renderer.h"

#include <sdl3/SDL.h>
#include <glad/gl.h>

#include "CommandList.h"
#include "Commands.h"
#include "RenderResources.h"
#include "Shader.h"

#include "exports.h"

void Renderer::clearCmd(Command* cmd)
{
	CmdClear* cCmd = (CmdClear*)cmd;

	glClearColor(cCmd->r, cCmd->g, cCmd->b, cCmd->a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::canvasCmd(Command* cmd)
{
	CmdCanvas* cCmd = (CmdCanvas*)cmd;

	if (cCmd->canvas == DEAD_GUID) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}

	RenderCanvasResource* canvas = m_Resources->getCanvas(cCmd->canvas);
	if (canvas == nullptr)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, canvas->fbo);
}

void Renderer::shaderCmd(Command* cmd)
{
	CmdShader* cCmd = (CmdShader*)cmd;

	if (cCmd->canvas == m_LastShader || cCmd->canvas == DEAD_GUID) {
		return;
	}

	m_LastShader = cCmd->canvas;

	RenderShaderResource* shader = m_Resources->getShader(cCmd->canvas);
	if (shader == nullptr)
		return;

	shader->shader->Bind();
}

Renderer::Renderer(SDL_Window* window, RenderResources* res)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	m_OpenGL = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, m_OpenGL);
	gladLoadGL(SDL_GL_GetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_DrawFunctions[0] = EVENT_HANDLER_BIND1(Renderer::clearCmd, this);
	m_DrawFunctions[1] = EVENT_HANDLER_BIND1(Renderer::canvasCmd, this);
	m_DrawFunctions[2] = EVENT_HANDLER_BIND1(Renderer::shaderCmd, this);

	m_CommandList = std::make_unique<CommandList>();
	m_Resources = res;
}

CommandList* Renderer::getCmds()
{
	return m_CommandList.get();
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(m_OpenGL);
}

CommandList* Renderer::beginCmd()
{
	return m_CommandList.get();
}

void Renderer::drawCmd()
{
	m_CommandList->canvas(DEAD_GUID);
	m_CommandList->clear(0.0f, 0.0f, 0.0f, 1.0f);

	auto& cmds = m_CommandList->retriveCmd();

	for (size_t i = 0; i < cmds.size(); i++)
	{
		m_DrawFunctions[cmds[i]->FuncIdx](cmds[i].get());
	}

	cmds.clear();
}

void Renderer::flushCmd()
{
	m_CommandList = std::make_unique<CommandList>();
}
