#include "pch.h"
#include "EventHandler.h"

#include <SDL3/SDL.h>

void EventHandler::HandleEvents()
{
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		for (size_t i = 0; i < m_Attachments.size(); i++)
		{
			m_Attachments[i](Event);
		}
	}
}

void EventHandler::Attach(std::function<void(SDL_Event&)> func)
{
	m_Attachments.push_back(func);
}
