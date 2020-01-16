#include "VerticalBlurPass.h"
#include "Sink.h"
#include "Source.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"

namespace Rgph
{
	VerticalBlurPass::VerticalBlurPass( std::string name,Graphics& gfx )
		:
		FullscreenPass( std::move( name ),gfx )
	{
		using namespace Bind;
		AddBind( PixelShader::Resolve( gfx,"BlurOutline_PS.cso" ) );
		AddBind( Blender::Resolve( gfx,true ) );
		AddBind( Stencil::Resolve( gfx,Stencil::Mode::Mask ) );

		AddBindSink<Bind::RenderTarget>( "scratchIn" );
		AddBindSink<Bind::CachingPixelConstantBufferEx>( "control" );
		RegisterSink( DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make( "direction",direction ) );
		RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );

		RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute( Graphics& gfx ) const noxnd
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer( buf );

		direction->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}
