#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Blender::Blender( Graphics& gfx,bool blending )
		:
		blending( blending )
	{
		INFOMAN( gfx );

		D3D11_BLEND_DESC blendDesc = {};
		auto& brt = blendDesc.RenderTarget[0];
		if( blending )
		{
			brt.BlendEnable = TRUE;
			brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			brt.BlendOp = D3D11_BLEND_OP_ADD;
			brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
			brt.DestBlendAlpha = D3D11_BLEND_ZERO;
			brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		else
		{
			brt.BlendEnable = FALSE;
			brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		GFX_THROW_INFO( GetDevice( gfx )->CreateBlendState( &blendDesc,&pBlender ) );
	}

	void Blender::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->OMSetBlendState( pBlender.Get(),nullptr,0xFFFFFFFFu );
	}
	
	std::shared_ptr<Blender> Blender::Resolve( Graphics& gfx,bool blending )
	{
		return Codex::Resolve<Blender>( gfx,blending );
	}
	std::string Blender::GenerateUID( bool blending )
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b" : "n");
	}
	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID( blending );
	}
}