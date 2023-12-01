#pragma once

#include <vector>
#include <memory>

#include <Guid.hpp>

struct Command;

class CommandList {
private:
	std::vector<std::shared_ptr<Command>> m_Commands;
public:
	void clear(float r, float g, float b, float a);
	void canvas(xg::Guid guid);
	void shader(xg::Guid guid);

	std::vector<std::shared_ptr<Command>>& retriveCmd();
};