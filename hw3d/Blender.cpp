#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Blender::Blender( Graphics& gfx,bool blending,std::optional<float> factors_in )
		:
		blending( blending )
	{
		INFOMAN( gfx );

		if( factors_in )
		{
			factors.emplace();
			factors->fill( *factors_in );
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& brt = blendDesc.RenderTarget[0];
		if( blending )
		{
			brt.BlendEnable = TRUE;

			if( factors_in )
			{
				brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}
		GFX_THROW_INFO( GetDevice( gfx )->CreateBlendState( &blendDesc,&pBlender ) );
	}

	void Blender::Bind( Graphics& gfx ) noexcept
	{
		const float* data = factors ? factors->data() : nullptr;
		GetContext( gfx )->OMSetBlendState( pBlender.Get(),data,0xFFFFFFFFu );
	}

	void Blender::SetFactor( float factor ) noxnd
	{
		assert( factors );
		return factors->fill( factor );
	}

	float Blender::GetFactor() const noxnd
	{
		assert( factors );
		return factors->front();
	}
	
	std::shared_ptr<Blender> Blender::Resolve( Graphics& gfx,bool blending,std::optional<float> factor )
	{
		return Codex::Resolve<Blender>( gfx,blending,factor );
	}
	std::string Blender::GenerateUID( bool blending,std::optional<float> factor )
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string( *factor ) : "");
	}
	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID( blending,factors ? factors->front() : std::optional<float>{} );
	}
}