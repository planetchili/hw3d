#include "HorizontalBlurPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Sink.h"
#include "Source.h"
#include "Blender.h"
#include "ConstantBuffersEx.h"

namespace Rgph
{
	HorizontalBlurPass::HorizontalBlurPass( std::string name,Graphics& gfx,unsigned int fullWidth,unsigned int fullHeight )
		:
		FullscreenPass( std::move( name ),gfx )
	{
		AddBind( Bind::PixelShader::Resolve( gfx,"BlurOutline_PS.cso" ) );
		AddBind( Bind::Blender::Resolve( gfx,false ) );
		
		AddBindSink<Bind::RenderTarget>( "scratchIn" );
		AddBindSink<Bind::CachingPixelConstantBufferEx>( "control" );
		RegisterSink( DirectBindableSink<Bind::CachingPixelConstantBufferEx>::Make( "direction",direction ) );

		// the renderTarget is internally sourced and then exported as a Bindable
		renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>( gfx,fullWidth / 2,fullHeight / 2,0u );
		RegisterSource( DirectBindableSource<Bind::RenderTarget>::Make( "scratchOut",renderTarget ) );
	}

	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)
	void HorizontalBlurPass::Execute( Graphics& gfx ) const noxnd
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer( buf );

		direction->Bind( gfx );
		FullscreenPass::Execute( gfx );
	}
}
