#include "Technique.h"
#include "Drawable.h"
#include "TechniqueProbe.h"

void Technique::Submit( const Drawable& drawable,size_t channelFilter ) const noexcept
{
	if( active && ((channels & channelFilter) != 0) )
	{
		for( const auto& step : steps )
		{
			step.Submit( drawable );
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

Technique::Technique( size_t channels )
	:
	channels{ channels }
{}

Technique::Technique( std::string name,size_t channels,bool startActive ) noexcept
	:
	active( startActive ),
	name( name ),
	channels( channels )
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

void Technique::Link( Rgph::RenderGraph& rg )
{
	for( auto& step : steps )
	{
		step.Link( rg );
	}
}
