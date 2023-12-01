#pragma once

#include <functional>

union SDL_Event;

class EventHandler {
private:
	std::vector<std::function<void(SDL_Event&)>> m_Handlers;
public:
	void bind(std::function<void(SDL_Event&)> handler);

	void process();
};