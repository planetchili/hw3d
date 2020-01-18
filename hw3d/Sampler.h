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
		Sampler( Graphics& gfx,Type type,bool reflect );
		void Bind( Graphics& gfx ) noxnd override;
		static std::shared_ptr<Sampler> Resolve( Graphics& gfx,Type type = Type::Anisotropic,bool reflect = false );
		static std::string GenerateUID( Type type,bool reflect );
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		Type type;
		bool reflect;
	};
}
