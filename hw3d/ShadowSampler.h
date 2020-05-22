#pragma once
#include "Bindable.h"

namespace Bind
{
	class ShadowSampler : public Bindable
	{
	public:
		ShadowSampler( Graphics& gfx );
		void Bind( Graphics& gfx ) noxnd override;
		void SetBilinear( bool bilin );
		void SetHwPcf( bool hwPcf );
		bool GetBilinear() const;
		bool GetHwPcf() const;
	private:
		UINT GetCurrentSlot() const;
		static size_t ShadowSamplerIndex( bool bilin,bool hwPcf );
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> MakeSampler( Graphics& gfx,bool bilin,bool hwPcf );
	protected:
		size_t curSampler;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[4];
	};
}
