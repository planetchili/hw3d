#pragma once
#include "Step.h"
#include <vector>

class TechniqueProbe;
class Drawable;

namespace Rgph
{
	class RenderGraph;
}

class Technique
{
public:
	Technique( size_t channels );
	Technique( std::string name,size_t channels,bool startActive = true ) noexcept;
	void Submit( const Drawable& drawable,size_t channels ) const noexcept;
	void AddStep( Step step ) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState( bool active_in ) noexcept;
	void InitializeParentReferences( const Drawable& parent ) noexcept;
	void Accept( TechniqueProbe& probe );
	const std::string& GetName() const noexcept;
	void Link( Rgph::RenderGraph& );
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name;
	size_t channels;
};