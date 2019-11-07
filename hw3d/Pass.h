#pragma once
#include "Graphics.h"
#include "Job.h"
#include <vector>

class Pass
{
public:
	void Accept( Job job ) noexcept;
	void Execute( Graphics& gfx ) const noxnd;
	void Reset() noexcept;
private:
	std::vector<Job> jobs;
};
