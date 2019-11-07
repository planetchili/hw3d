#pragma once
#include "Step.h"
#include <vector>

class TechniqueProbe;

class Technique
{
public:
	Technique() = default;
	Technique( std::string name,bool startActive = true ) noexcept;
	void Submit( class FrameCommander& frame,const class Drawable& drawable ) const noexcept;
	void AddStep( Step step ) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState( bool active_in ) noexcept;
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe );
	const std::string& GetName() const noexcept;
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name = "Nameless Tech";
};