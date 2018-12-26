#pragma once
#include <chrono>

class ChiliTimer
{
public:
	ChiliTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};