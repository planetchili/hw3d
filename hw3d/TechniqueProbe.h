#pragma once

namespace Dcb
{
	class Buffer;
}

class TechniqueProbe
{
public:
	// TODO: add callback for visiting each mesh

	void SetTechnique( class Technique* pTech_in )
	{
		pTech = pTech_in;
		techIdx++;
		OnSetTechnique();
	}
	void SetStep( class Step* pStep_in )
	{
		pStep = pStep_in;
		stepIdx++;
		OnSetStep();
	}
	bool VisitBuffer( class Dcb::Buffer& buf )
	{
		bufIdx++;
		return OnVisitBuffer( buf );
	}
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
	virtual bool OnVisitBuffer( class Dcb::Buffer& )
	{
		return false;
	}
protected:
	class Technique* pTech = nullptr;
	class Step* pStep = nullptr;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};