#include "Pass.h"


void Pass::Accept( Job job ) noexcept
{
	jobs.push_back( job );
}

void Pass::Execute( Graphics& gfx ) const noxnd
{
	for( const auto& j : jobs )
	{
		j.Execute( gfx );
	}
}

void Pass::Reset() noexcept
{
	jobs.clear();
}