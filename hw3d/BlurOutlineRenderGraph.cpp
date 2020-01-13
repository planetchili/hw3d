#include "BlurOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"
#include "PassOutput.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "BlurOutlineDrawingPass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "ChiliMath.h"

BlurOutlineRenderGraph::BlurOutlineRenderGraph( Graphics& gfx )
	:
	RenderGraph( gfx )
{
	{
		auto pass = std::make_unique<BufferClearPass>( "clear" );
		pass->SetInputSource( "renderTarget","$.backbuffer" );
		pass->SetInputSource( "depthStencil","$.masterDepth" );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<LambertianPass>( gfx,"lambertian" );
		pass->SetInputSource( "renderTarget","clear.renderTarget" );
		pass->SetInputSource( "depthStencil","clear.depthStencil" );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<OutlineMaskGenerationPass>( gfx,"outlineMask" );
		pass->SetInputSource( "depthStencil","lambertian.depthStencil" );
		AppendPass( std::move( pass ) );
	}

	// setup blur constant buffers
	{
		{
			Dcb::RawLayout l;
			l.Add<Dcb::Integer>( "nTaps" );
			l.Add<Dcb::Array>( "coefficients" );
			l["coefficients"].Set<Dcb::Float>( maxRadius * 2 + 1 );
			Dcb::Buffer buf{ std::move( l ) };
			blurControl = std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx,buf,0 );
			SetKernelGauss( radius,sigma );
			AddGlobalSource( ImmutableOutput<Bind::CachingPixelConstantBufferEx>::Make( "blurControl",blurControl ) );
		}
		{
			Dcb::RawLayout l;
			l.Add<Dcb::Bool>( "isHorizontal" );
			Dcb::Buffer buf{ std::move( l ) };
			blurDirection = std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx,buf,1 );
			AddGlobalSource( ImmutableOutput<Bind::CachingPixelConstantBufferEx>::Make( "blurDirection",blurDirection ) );
		}
	}

	{
		auto pass = std::make_unique<BlurOutlineDrawingPass>( gfx,"outlineDraw",gfx.GetWidth(),gfx.GetHeight() );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<HorizontalBlurPass>( "horizontal",gfx,gfx.GetWidth(),gfx.GetHeight() );
		pass->SetInputSource( "scratchIn","outlineDraw.scratchOut" );
		pass->SetInputSource( "control","$.blurControl" );
		pass->SetInputSource( "direction","$.blurDirection" );
		AppendPass( std::move( pass ) );
	}
	{
		auto pass = std::make_unique<VerticalBlurPass>( "vertical",gfx );
		pass->SetInputSource( "renderTarget","lambertian.renderTarget" );
		pass->SetInputSource( "depthStencil","outlineMask.depthStencil" );
		pass->SetInputSource( "scratchIn","horizontal.scratchOut" );
		pass->SetInputSource( "control","$.blurControl" );
		pass->SetInputSource( "direction","$.blurDirection" );
		AppendPass( std::move( pass ) );
	}
	SetSinkTarget( "backbuffer","vertical.renderTarget" );

	Finalize();
}

void BlurOutlineRenderGraph::SetKernelGauss( int radius,float sigma ) noxnd
{
	assert( radius <= maxRadius );
	auto k = blurControl->GetBuffer();
	const int nTaps = radius * 2 + 1;
	k["nTaps"] = nTaps;
	float sum = 0.0f;
	for( int i = 0; i < nTaps; i++ )
	{
		const auto x = float( i - radius );
		const auto g = gauss( x,sigma );
		sum += g;
		k["coefficients"][i] = g;
	}
	for( int i = 0; i < nTaps; i++ )
	{
		k["coefficients"][i] = (float)k["coefficients"][i] / sum;
	}
	blurControl->SetBuffer( k );
}
