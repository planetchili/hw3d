#pragma once

namespace Dcb
{
	class Buffer;
}

class TechniqueProbe
{
public:
	void SetTechnique( class Technique* pTech_in )
	{
		pTech = pTech_in;
		OnSetTechnique();
	}
	void SetStep( class Step* pStep_in )
	{
		pStep = pStep_in;
		OnSetStep();
	}
	virtual bool VisitBuffer( class Dcb::Buffer& ) = 0;
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
protected:
	class Technique* pTech = nullptr;
	class Step* pStep = nullptr;
};