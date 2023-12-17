#pragma once

#include <string>

struct SDL_Window;
union SDL_Event;

class Window {
private:
	SDL_Window* m_Window;

	int m_WindowId;

	int m_Width;
	int m_Height;

	std::string m_Title;

	bool m_Closed;
public:
	Window();
	~Window();

	void setTitle(char* title);
	void setWidth(int width);
	void setHeight(int height);

	const char* getTitle(bool real = false);
	int getWidth(bool real = false);
	int getHeight(bool real = false);

	void swapWindow();

	void eventProc(SDL_Event& Event);

	bool isClosed();

	SDL_Window* getWindow();
};