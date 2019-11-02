#pragma once
#include "BindableCommon.h"
#include "ChiliMath.h"

class BlurPack
{
public:
	BlurPack( Graphics& gfx,int radius = 7,float sigma = 2.6f )
		:
		shader( gfx,"Blur_PS.cso" ),
		pcb( gfx,0u ),
		ccb( gfx,1u )
	{
		SetKernel( gfx,radius,sigma );
	}
	void Bind( Graphics& gfx ) noexcept
	{
		shader.Bind( gfx );
		pcb.Bind( gfx );
		ccb.Bind( gfx );
	}
	void SetHorizontal( Graphics& gfx )
	{
		ccb.Update( gfx,{ TRUE } );
	}
	void SetVertical( Graphics& gfx )
	{
		ccb.Update( gfx,{ FALSE } );
	}
	// nTaps should be 6sigma - 1
	// for more accurate coefs, need to integrate, but meh :/
	void SetKernel( Graphics& gfx,int radius,float sigma ) noxnd
	{
		assert( radius <= 7 );
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.0f;
		for( int i = 0; i < k.nTaps; i++ )
		{
			const auto x = float(i - radius);
			const auto g = gauss( x,sigma );
			sum += g;
			k.coefficients[i].x = g;
		}
		for( int i = 0; i < k.nTaps; i++ )
		{
			k.coefficients[i].x /= sum;
		}
		pcb.Update( gfx,k );
	}
private:
	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[15];
	};
	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};
	Bind::PixelShader shader;
	Bind::PixelConstantBuffer<Kernel> pcb;
	Bind::PixelConstantBuffer<Control> ccb;
};