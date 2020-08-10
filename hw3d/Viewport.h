#pragma once
#include "Bindable.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	class Viewport : public Bindable
	{
	public:
		Viewport( Graphics& gfx )
			:
			Viewport( gfx,(float)gfx.GetWidth(),(float)gfx.GetHeight() )
		{}
		Viewport( Graphics& gfx,float width,float height )
		{
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
		}
		void Bind( Graphics& gfx ) noxnd override
		{
			INFOMAN_NOHR( gfx );
			GetContext( gfx )->RSSetViewports( 1u,&vp );
		}
	private:
		D3D11_VIEWPORT vp{};
	};
}
