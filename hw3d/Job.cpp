#include "Job.h"
#include "Step.h"
#include "Drawable.h"


Job::Job( const Step* pStep,const Drawable* pDrawable )
	:
	pDrawable{ pDrawable },
	pStep{ pStep }
{}

void Job::Execute( Graphics& gfx ) const noxnd
{
	pDrawable->Bind( gfx );
	pStep->Bind( gfx );
	gfx.DrawIndexed( pDrawable->GetIndexCount() );
}