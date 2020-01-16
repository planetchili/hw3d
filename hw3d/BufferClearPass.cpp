#include "BufferClearPass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Sink.h"
#include "Source.h"


namespace Rgph
{
	BufferClearPass::BufferClearPass( std::string name )
		:
		Pass( std::move( name ) )
	{
		RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
		RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
	}

	void BufferClearPass::Execute( Graphics& gfx ) const noxnd
	{
		renderTarget->Clear( gfx );
		depthStencil->Clear( gfx );
	}
}