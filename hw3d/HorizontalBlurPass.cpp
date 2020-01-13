#include "HorizontalBlurPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "PassInput.h"
#include "PassOutput.h"
#include "Blender.h"

HorizontalBlurPass::HorizontalBlurPass( std::string name,Graphics& gfx,unsigned int fullWidth,unsigned int fullHeight )
	:
	FullscreenPass( std::move( name ),gfx )
{
	AddBind( Bind::PixelShader::Resolve( gfx,"BlurOutline_PS.cso" ) );
	AddBind( Bind::Blender::Resolve( gfx,false ) );

	RegisterInput( ImmutableInput<Bind::Bindable>::Make( "control",control ) );
	RegisterInput( ImmutableInput<Bind::CachingPixelConstantBufferEx>::Make( "direction",direction ) );
	RegisterInput( ImmutableInput<Bind::Bindable>::Make( "scratchIn",blurScratchIn ) );

	// the renderTarget is internally sourced and then exported as a Bindable
	renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>( gfx,fullWidth / 2,fullHeight / 2,0u );
	RegisterOutput( ImmutableOutput<Bind::RenderTarget>::Make( "scratchOut",renderTarget ) );
}

// this override is necessary because we cannot (yet) link input bindables directly into
// the container of bindables (mainly because vector growth buggers references)
void HorizontalBlurPass::Execute( Graphics& gfx ) const noexcept
{
	auto buf = direction->GetBuffer();
	buf["isHorizontal"] = true;
	direction->SetBuffer( buf );

	blurScratchIn->Bind( gfx );
	control->Bind( gfx );
	direction->Bind( gfx );
	FullscreenPass::Execute( gfx );
}
