#include "Technique.h"
#include "Drawable.h"
#include "FrameCommander.h"
#include "TechniqueProbe.h"

void Technique::Submit( FrameCommander& frame,const Drawable& drawable ) const noexcept
{
	if( active )
	{
		for( const auto& step : steps )
		{
			step.Submit( frame,drawable );
		}
	}
}

void Technique::InitializeParentReferences( const Drawable & parent ) noexcept
{
	for( auto& s : steps )
	{
		s.InitializeParentReferences( parent );
	}
}

Technique::Technique( std::string name,bool startActive ) noexcept
	:
	active( startActive ),
	name( name )
{}

void Technique::AddStep( Step step ) noexcept
{
	steps.push_back( std::move( step ) );
}

bool Technique::IsActive() const noexcept
{
	return active;
}

void Technique::SetActiveState( bool active_in ) noexcept
{
	active = active_in;
}

void Technique::Accept( TechniqueProbe& probe )
{
	probe.SetTechnique( this );
	for( auto& s : steps )
	{
		s.Accept( probe );
	}
}

const std::string& Technique::GetName() const noexcept
{
	return name;
}
