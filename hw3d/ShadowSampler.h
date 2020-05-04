#pragma once
#include "Bindable.h"

namespace Bind
{
	class ShadowSampler : public Bindable
	{
	public:
		ShadowSampler( Graphics& gfx );
		void Bind( Graphics& gfx ) noxnd override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}
