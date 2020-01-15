#include "VerticalBlurPass.h"
#include "PassInput.h"
#include "PassOutput.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"


VerticalBlurPass::VerticalBlurPass( std::string name,Graphics& gfx )
	:
	FullscreenPass( std::move( name ),gfx )
{
	using namespace Bind;
	AddBind( PixelShader::Resolve( gfx,"BlurOutline_PS.cso" ) );
	AddBind( Blender::Resolve( gfx,true ) );
	AddBind( Stencil::Resolve( gfx,Stencil::Mode::Mask ) );

	RegisterInput( ImmutableInput<Bind::Bindable>::Make( "scratchIn",blurScratchIn ) );
	RegisterInput( ImmutableInput<Bind::Bindable>::Make( "control",control ) );
	RegisterInput( ImmutableInput<Bind::CachingPixelConstantBufferEx>::Make( "direction",direction ) );
	RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
	RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );

	RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
	RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
}

// see the note on HorizontalBlurPass::Execute
void VerticalBlurPass::Execute( Graphics& gfx ) const noxnd
{
	auto buf = direction->GetBuffer();
	buf["isHorizontal"] = false;
	direction->SetBuffer( buf );

	control->Bind( gfx );
	direction->Bind( gfx );
	blurScratchIn->Bind( gfx );
	FullscreenPass::Execute( gfx );
}
