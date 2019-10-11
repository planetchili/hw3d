#pragma once
#include "Bindable.h"

namespace Bind
{
	class Stencil : public Bindable
	{
	public:
		enum class Mode
		{
			Off,
			Write,
			Mask
		};
		Stencil( Graphics& gfx,Mode mode )
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

			if( mode == Mode::Write )
			{
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			}
			else if( mode == Mode::Mask )
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilReadMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			}

			GetDevice( gfx )->CreateDepthStencilState( &dsDesc,&pStencil );
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->OMSetDepthStencilState( pStencil.Get(),0xFF );
		}
		//static std::shared_ptr<Stencil> Resolve( Graphics& gfx,bool blending,std::optional<float> factor = {} );
		//static std::string GenerateUID( bool blending,std::optional<float> factor );
		//std::string GetUID() const noexcept override;
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
	};
}
