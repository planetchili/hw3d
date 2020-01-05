#include "BufferClearPass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "PassInput.h"
#include "PassOutput.h"


BufferClearPass::BufferClearPass( std::string name )
	:
	Pass( std::move( name ) )
{
	RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
	RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
	RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
	RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
}

void BufferClearPass::Execute( Graphics& gfx ) const noxnd
{
	renderTarget->Clear( gfx );
	depthStencil->Clear( gfx );
}