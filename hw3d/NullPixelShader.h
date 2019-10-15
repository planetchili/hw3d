#pragma once
#include "Bindable.h"

namespace Bind
{
	class NullPixelShader : public Bindable
	{
	public:
		NullPixelShader( Graphics& gfx );
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<NullPixelShader> Resolve( Graphics& gfx );
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	};
}