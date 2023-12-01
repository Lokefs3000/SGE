#include "pch.h"
#include "Window.h"

#include <sdl3/SDL.h>

Window::Window()
{
	m_Window = SDL_CreateWindow("Untitled", 800, 600, SDL_WINDOW_OPENGL);

	m_Width = 800;
	m_Height = 600;

	m_WindowId = SDL_GetWindowID(m_Window);

	m_Closed = false;
}

Window::~Window()
{
	SDL_DestroyWindow(m_Window);
}

void Window::setTitle(char* title)
{
	SDL_SetWindowTitle(m_Window, title);
}

void Window::setWidth(int width)
{
	SDL_SetWindowSize(m_Window, width, m_Height);
	m_Width = width;
}

void Window::setHeight(int height)
{
	SDL_SetWindowSize(m_Window, m_Width, height);
	m_Height = height;
}

void Window::swapWindow()
{
	SDL_GL_SwapWindow(m_Window);
}

void Window::eventProc(SDL_Event& Event)
{
	if (Event.window.windowID == m_WindowId) {
		switch (Event.type)
		{
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			m_Closed = true;
			break;
		default:
			break;
		}
	}
}

bool Window::isClosed()
{
	return m_Closed;
}

SDL_Window* Window::getWindow()
{
	return m_Window;
}
