#include "ChiliTimer.h"

using namespace std::chrono;

ChiliTimer::ChiliTimer() noexcept
{
	last = steady_clock::now();
}

float ChiliTimer::Mark() noexcept
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float ChiliTimer::Peek() const noexcept
{
	return duration<float>( steady_clock::now() - last ).count();
}
