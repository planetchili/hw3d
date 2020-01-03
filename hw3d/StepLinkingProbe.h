#pragma once
#include "TechniqueProbe.h"

class RenderGraph;

class StepLinkingProbe : public TechniqueProbe
{
protected:
	void OnSetStep() override;
private:
	RenderGraph& rg;
};