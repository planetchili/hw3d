#pragma once
#include "Step.h"
#include <vector>


class Technique
{
public:
	void Submit( class FrameCommander& frame,const class Drawable& drawable ) const noexcept;
	void AddStep( Step step ) noexcept
	{
		steps.push_back( std::move( step ) );
	}
	void Activate() noexcept
	{
		active = true;
	}
	void Deactivate() noexcept
	{
		active = false;
	}
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
private:
	bool active = true;
	std::vector<Step> steps;
};