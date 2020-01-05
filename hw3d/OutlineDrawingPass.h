#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"

class Graphics;

class OutlineDrawingPass : public RenderQueuePass
{
public:
	OutlineDrawingPass( Graphics& gfx,std::string name )
		:
		RenderQueuePass( std::move( name ) )
	{
		using namespace Bind;
		RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
		RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterOutput( BufferOutput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
		AddBind( VertexShader::Resolve( gfx,"Solid_VS.cso" ) );
		AddBind( PixelShader::Resolve( gfx,"Solid_PS.cso" ) );
		AddBind( Stencil::Resolve( gfx,Stencil::Mode::Mask ) );
		AddBind( Rasterizer::Resolve( gfx,false ) );
	}
};