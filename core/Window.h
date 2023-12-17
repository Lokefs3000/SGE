#pragma once

#include "globals.h"

#include <string>
#include <memory>

struct SDL_Window;
class DataElement;

class Window {
private:
	SDL_Window* m_Window;
public:
	Window(std::shared_ptr<DataElement> data);
	~Window();

	void PerformBufferSwap();

	SDL_Window* GetInternalWindow();
};