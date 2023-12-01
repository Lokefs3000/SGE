#pragma once

#include <Guid.hpp>

struct Command {
	uint32_t FuncIdx;
};

struct CmdClear : public Command {
	float r;
	float g;
	float b;
	float a;
};

struct CmdCanvas : public Command {
	xg::Guid canvas;
};

struct CmdShader : public Command {
	xg::Guid canvas;
};