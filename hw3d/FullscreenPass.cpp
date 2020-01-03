#include "FullscreenPass.h"
#include "BindableCommon.h"


namespace dx = DirectX;

FullscreenPass::FullscreenPass( const std::string name,Graphics& gfx ) noxnd
	:
	BindingPass( std::move( name ) )
{
	// do static initialization if required
	if( !indexBuffer )
	{
		// setup fullscreen geometry
		Dvtx::VertexLayout lay;
		lay.Append( Dvtx::VertexLayout::Position2D );
		Dvtx::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack( dx::XMFLOAT2{ -1,1 } );
		bufFull.EmplaceBack( dx::XMFLOAT2{ 1,1 } );
		bufFull.EmplaceBack( dx::XMFLOAT2{ -1,-1 } );
		bufFull.EmplaceBack( dx::XMFLOAT2{ 1,-1 } );
		vertexBuffer = Bind::VertexBuffer::Resolve( gfx,"$Full",std::move( bufFull ) );
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		indexBuffer = Bind::IndexBuffer::Resolve( gfx,"$Full",std::move( indices ) );
		// setup fullscreen shaders
		vertexShader = Bind::VertexShader::Resolve( gfx,"Fullscreen_VS.cso" );
		inputLayout = Bind::InputLayout::Resolve( gfx,lay,vertexShader->GetBytecode() );
	}
}

void FullscreenPass::Execute( Graphics& gfx ) const noexcept
{
	BindAll( gfx );
	vertexBuffer->Bind( gfx );
	indexBuffer->Bind( gfx );
	vertexBuffer->Bind( gfx );
	inputLayout->Bind( gfx );

	gfx.DrawIndexed( indexBuffer->GetCount() );
}

std::shared_ptr<Bind::VertexBuffer> FullscreenPass::vertexBuffer;
std::shared_ptr<Bind::IndexBuffer> FullscreenPass::indexBuffer;
std::shared_ptr<Bind::VertexShader> FullscreenPass::vertexShader;
std::shared_ptr<Bind::InputLayout> FullscreenPass::inputLayout;
