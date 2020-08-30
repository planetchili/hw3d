#pragma once
#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		enum class Type
		{
			Anisotropic,
			Bilinear,
			Point,
		};
	public:
		Sampler( Graphics& gfx,Type type,bool reflect,UINT slot );
		void Bind( Graphics& gfx ) noxnd override;
		static std::shared_ptr<Sampler> Resolve( Graphics& gfx,Type type = Type::Anisotropic,bool reflect = false,UINT slot = 0u );
		static std::string GenerateUID( Type type,bool reflect,UINT slot );
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		Type type;
		bool reflect;
		UINT slot;
	};
}
