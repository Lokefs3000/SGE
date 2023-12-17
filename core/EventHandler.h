#pragma once

#include "globals.h"

#include <functional>

union SDL_Event;

class EventHandler {
private:
	std::vector<std::function<void(SDL_Event&)>> m_Attachments;
public:
	void HandleEvents();

	void Attach(std::function<void(SDL_Event&)> func);
};