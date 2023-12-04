#include "pch.h"
#include "TimeHandler.h"

TimeHandler::TimeHandler()
{
	m_LastFrame = std::chrono::high_resolution_clock::now();
	m_Delta = 0.0;
}

void TimeHandler::retimeDelta()
{
	using ms = std::chrono::duration<double, std::milli>;
	m_Delta = std::chrono::duration_cast<ms>(std::chrono::high_resolution_clock::now() - m_LastFrame).count() / 1000.0f;
	m_LastFrame = std::chrono::high_resolution_clock::now();
}

double TimeHandler::delta()
{
	return m_Delta;
}
