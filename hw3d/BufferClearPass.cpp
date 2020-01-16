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
		RegisterSink( DirectBufferSink<Bind::BufferResource>::Make( "buffer",buffer ) );
		RegisterSource( DirectBufferSource<Bind::BufferResource>::Make( "buffer",buffer ) );
	}

	void BufferClearPass::Execute( Graphics& gfx ) const noxnd
	{
		buffer->Clear( gfx );
	}
}