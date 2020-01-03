#include "RenderQueuePass.h"


void RenderQueuePass::Accept( Job job ) noexcept
{
	jobs.push_back( job );
}

void RenderQueuePass::Execute( Graphics& gfx ) const noxnd
{
	BindAll( gfx );

	for( const auto& j : jobs )
	{
		j.Execute( gfx );
	}
}

void RenderQueuePass::Reset() noxnd
{
	jobs.clear();
}
