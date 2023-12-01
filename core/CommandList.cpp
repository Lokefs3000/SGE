#include "pch.h"
#include "CommandList.h"

#include "Commands.h"

void CommandList::clear(float r, float g, float b, float a)
{
	std::shared_ptr<CmdClear> cmd = std::make_shared<CmdClear>();

	cmd->r = r;
	cmd->g = g;
	cmd->b = b;
	cmd->a = a;
	cmd->FuncIdx = 0;

	m_Commands.push_back(cmd);
}

void CommandList::canvas(xg::Guid guid)
{
	std::shared_ptr<CmdCanvas> cmd = std::make_shared<CmdCanvas>();

	cmd->canvas = guid;
	cmd->FuncIdx = 1;

	m_Commands.push_back(cmd);
}

void CommandList::shader(xg::Guid guid)
{
	std::shared_ptr<CmdShader> cmd = std::make_shared<CmdShader>();

	cmd->canvas = guid;
	cmd->FuncIdx = 2;

	m_Commands.push_back(cmd);
}

std::vector<std::shared_ptr<Command>>& CommandList::retriveCmd()
{
	return m_Commands;
}
