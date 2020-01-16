#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class Graphics;

namespace Rgph
{
	class OutlineDrawingPass : public RenderQueuePass
	{
	public:
		OutlineDrawingPass( Graphics& gfx,std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;
			RegisterSink( DirectBufferSink<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
			RegisterSink( DirectBufferSink<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
			RegisterSource( DirectBufferSource<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
			RegisterSource( DirectBufferSource<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
			AddBind( VertexShader::Resolve( gfx,"Solid_VS.cso" ) );
			AddBind( PixelShader::Resolve( gfx,"Solid_PS.cso" ) );
			AddBind( Stencil::Resolve( gfx,Stencil::Mode::Mask ) );
			AddBind( Rasterizer::Resolve( gfx,false ) );
		}
	};
}