#include "SkyboxTransformCbuf.h"

namespace Bind
{
	SkyboxTransformCbuf::SkyboxTransformCbuf( Graphics& gfx,UINT slot )
		:
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transforms>>( gfx,slot ) }
	{}

	void SkyboxTransformCbuf::Bind( Graphics& gfx ) noxnd
	{
		INFOMAN_NOHR( gfx );
		GFX_THROW_INFO_ONLY( UpdateBindImpl( gfx,GetTransforms( gfx ) ) );
	}

	void SkyboxTransformCbuf::UpdateBindImpl( Graphics& gfx,const Transforms& tf ) noxnd
	{
		pVcbuf->Update( gfx,tf );
		pVcbuf->Bind( gfx );
	}

	SkyboxTransformCbuf::Transforms SkyboxTransformCbuf::GetTransforms( Graphics& gfx ) noxnd
	{
		return {
			DirectX::XMMatrixTranspose( gfx.GetCamera() * gfx.GetProjection() )
		};
	}
}