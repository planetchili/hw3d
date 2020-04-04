#include "Frustum.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"

namespace dx = DirectX;

Frustum::Frustum( Graphics& gfx,float width,float height,float nearZ,float farZ )
{
	using namespace Bind;

	std::vector<unsigned short> indices;
	{
		indices.push_back( 0 );
		indices.push_back( 1 );
		indices.push_back( 1 );
		indices.push_back( 2 );
		indices.push_back( 2 );
		indices.push_back( 3 );
		indices.push_back( 3 );
		indices.push_back( 0 );
		indices.push_back( 4 );
		indices.push_back( 5 );
		indices.push_back( 5 );
		indices.push_back( 6 );
		indices.push_back( 6 );
		indices.push_back( 7 );
		indices.push_back( 7 );
		indices.push_back( 4 );
		indices.push_back( 0 );
		indices.push_back( 4 );
		indices.push_back( 1 );
		indices.push_back( 5 );
		indices.push_back( 2 );
		indices.push_back( 6 );
		indices.push_back( 3 );
		indices.push_back( 7 );
	}

	SetVertices( gfx,width,height,nearZ,farZ );
	pIndices = IndexBuffer::Resolve( gfx,"$frustum",indices );
	pTopology = Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	{
		Technique line;
		{
			Step unoccluded( "lambertian" );

			auto pvs = VertexShader::Resolve( gfx,"Solid_VS.cso" );
			unoccluded.AddBindable( InputLayout::Resolve( gfx,pVertices->GetLayout(),*pvs ) );
			unoccluded.AddBindable( std::move( pvs ) );

			unoccluded.AddBindable( PixelShader::Resolve( gfx,"Solid_PS.cso" ) );

			struct PSColorConstant
			{
				dx::XMFLOAT3 color = { 0.6f,0.2f,0.2f };
				float padding;
			} colorConst;
			unoccluded.AddBindable( PixelConstantBuffer<PSColorConstant>::Resolve( gfx,colorConst,1u ) );

			unoccluded.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			unoccluded.AddBindable( Rasterizer::Resolve( gfx,false ) );

			line.AddStep( std::move( unoccluded ) );
		}
		{
			Step occluded( "wireframe" );

			auto pvs = VertexShader::Resolve( gfx,"Solid_VS.cso" );
			occluded.AddBindable( InputLayout::Resolve( gfx,pVertices->GetLayout(),*pvs ) );
			occluded.AddBindable( std::move( pvs ) );

			occluded.AddBindable( PixelShader::Resolve( gfx,"Solid_PS.cso" ) );

			struct PSColorConstant2
			{
				dx::XMFLOAT3 color = { 0.25f,0.08f,0.08f };
				float padding;
			} colorConst;
			occluded.AddBindable( PixelConstantBuffer<PSColorConstant2>::Resolve( gfx,colorConst,1u ) );

			occluded.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			occluded.AddBindable( Rasterizer::Resolve( gfx,false ) );

			line.AddStep( std::move( occluded ) );
		}
		AddTechnique( std::move( line ) );
	}
}

void Frustum::SetVertices( Graphics& gfx,float width,float height,float nearZ,float farZ )
{
	Dvtx::VertexLayout layout;
	layout.Append( Dvtx::VertexLayout::Position3D );
	Dvtx::VertexBuffer vertices{ std::move( layout ) };
	{
		const float zRatio = farZ / nearZ;
		const float nearX = width / 2.0f;
		const float nearY = height / 2.0f;
		const float farX = nearX * zRatio;
		const float farY = nearY * zRatio;
		vertices.EmplaceBack( dx::XMFLOAT3{ -nearX,nearY,nearZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ nearX,nearY,nearZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ nearX,-nearY,nearZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ -nearX,-nearY,nearZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ -farX,farY,farZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ farX,farY,farZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ farX,-farY,farZ } );
		vertices.EmplaceBack( dx::XMFLOAT3{ -farX,-farY,farZ } );
	}
	pVertices = std::make_shared<Bind::VertexBuffer>( gfx,vertices );
}

void Frustum::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void Frustum::SetRotation( DirectX::XMFLOAT3 rot ) noexcept
{
	this->rot = rot;
}

DirectX::XMMATRIX Frustum::GetTransformXM() const noexcept
{
	return dx::XMMatrixRotationRollPitchYawFromVector( dx::XMLoadFloat3( &rot ) ) *
		dx::XMMatrixTranslationFromVector( dx::XMLoadFloat3( &pos ) );
}