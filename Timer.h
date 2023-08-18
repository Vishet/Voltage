#pragma once
#include <chrono>

class Timer
{
private:
	std::chrono::steady_clock::time_point last;

public:
	Timer();
	float Mark();
	float Peek() const;
};

