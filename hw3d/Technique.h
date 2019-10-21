#pragma once
#include "Step.h"
#include "TechniqueProbe.h"
#include <vector>


class Technique
{
public:
	Technique() = default;
	Technique( std::string name,bool startActive = true ) noexcept
		:
		active( startActive ),
		name( name )
	{}
	void Submit( class FrameCommander& frame,const class Drawable& drawable ) const noexcept;
	void AddStep( Step step ) noexcept
	{
		steps.push_back( std::move( step ) );
	}
	bool IsActive() const noexcept
	{
		return active;
	}
	void SetActiveState( bool active_in ) noexcept
	{
		active = active_in;
	}
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe )
	{
		probe.SetTechnique( this );
		for( auto& s : steps )
		{
			s.Accept( probe );
		}
	}
	const std::string& GetName() const noexcept
	{
		return name;
	}
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name = "Nameless Tech";
};