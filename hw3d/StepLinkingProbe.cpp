#include "StepLinkingProbe.h"
#include "RenderGraph.h"
#include "Step.h"

void StepLinkingProbe::OnSetStep()
{
	pStep->Link( rg );
}