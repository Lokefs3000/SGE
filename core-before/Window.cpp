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
	m_Title = title;
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

const char* Window::getTitle(bool real)
{
	if (real) {
		return SDL_GetWindowTitle(m_Window);
	}
	else {
		return m_Title.c_str();
	}
}

int Window::getWidth(bool real)
{
	int w = 0;
	SDL_GetWindowSizeInPixels(m_Window, &w, NULL);
	return w;
}

int Window::getHeight(bool real)
{
	int h = 0;
	SDL_GetWindowSizeInPixels(m_Window, NULL, &h);
	return h;
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
