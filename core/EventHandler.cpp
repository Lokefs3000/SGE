#include "pch.h"
#include "EventHandler.h"

#include <sdl3/SDL.h>

void EventHandler::bind(std::function<void(SDL_Event&)> handler)
{
	m_Handlers.push_back(handler);
}

void EventHandler::process()
{
	SDL_Event Event;

	while (SDL_PollEvent(&Event))
	{
		for (size_t i = 0; i < m_Handlers.size(); i++)
		{
			m_Handlers[i](Event);
		}
	}
}
