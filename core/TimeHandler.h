#pragma once

#include <chrono>

class TimeHandler {
private:
	std::chrono::high_resolution_clock::time_point m_LastFrame;

	double m_Delta; //Double precision baby!
public:
	TimeHandler();

	void retimeDelta();

	double delta();
};