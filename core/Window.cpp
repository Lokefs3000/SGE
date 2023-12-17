#include "pch.h"
#include "Window.h"

#include <SDL3/SDL.h>

#include "DataParser.h"

Window::Window(std::shared_ptr<DataElement> data)
{
	std::shared_ptr<DataElement> winConf = data->GetChild("config")->GetChild("window");
	m_Window = SDL_CreateWindow(winConf->GetValue("title").ValString.c_str(), winConf->GetValue("width").ValInt, winConf->GetValue("height").ValInt, SDL_WINDOW_OPENGL);
}

Window::~Window()
{
	SDL_DestroyWindow(m_Window);
}

void Window::PerformBufferSwap()
{
	SDL_GL_SwapWindow(m_Window);
}

SDL_Window* Window::GetInternalWindow()
{
	return m_Window;
}
