#include "ScaleOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"

ScaleOutlineRenderGraph::ScaleOutlineRenderGraph( Graphics& gfx )
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
	{
		auto pass = std::make_unique<OutlineDrawingPass>( gfx,"outlineDraw" );
		pass->SetInputSource( "renderTarget","lambertian.renderTarget" );
		pass->SetInputSource( "depthStencil","outlineMask.depthStencil" );
		AppendPass( std::move( pass ) );
	}
	SetSinkTarget( "backbuffer","outlineDraw.renderTarget" );
	Finalize();
}
