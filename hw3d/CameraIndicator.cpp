#include "CameraIndicator.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"
#include "Channels.h"

namespace dx = DirectX;

CameraIndicator::CameraIndicator( Graphics& gfx )
{
	using namespace Bind;

	const auto geometryTag = "$cam";
	Dvtx::VertexLayout layout;
	layout.Append( Dvtx::VertexLayout::Position3D );
	Dvtx::VertexBuffer vertices{ std::move( layout ) };
	{
		const float x = 4.0f / 3.0f * 0.75f;
		const float y = 1.0f * 0.75f;
		const float z = -2.0f;
		const float thalf = x * 0.5f;
		const float tspace = y * 0.2f;
		vertices.EmplaceBack( dx::XMFLOAT3{ -x,y,0.0f } );
		vertices.EmplaceBack( dx::XMFLOAT3{ x,y,0.0f } );
		vertices.EmplaceBack( dx::XMFLOAT3{ x,-y,0.0f } );
		vertices.EmplaceBack( dx::XMFLOAT3{ -x,-y,0.0f } );
		vertices.EmplaceBack( dx::XMFLOAT3{ 0.0f,0.0f,z } );
		vertices.EmplaceBack( dx::XMFLOAT3{ -thalf,y + tspace,0.0f } );
		vertices.EmplaceBack( dx::XMFLOAT3{ thalf,y + tspace,0.0f } );
		vertices.EmplaceBack( dx::XMFLOAT3{ 0.0f,y + tspace + thalf,0.0f } );
	}
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
		indices.push_back( 0 );
		indices.push_back( 4 );
		indices.push_back( 1 );
		indices.push_back( 4 );
		indices.push_back( 2 );
		indices.push_back( 4 );
		indices.push_back( 3 );
		indices.push_back( 4 );
		indices.push_back( 5 );
		indices.push_back( 6 );
		indices.push_back( 6 );
		indices.push_back( 7 );
		indices.push_back( 7 );
		indices.push_back( 5 );
	}

	pVertices = VertexBuffer::Resolve( gfx,geometryTag,vertices );
	pIndices = IndexBuffer::Resolve( gfx,geometryTag,indices );
	pTopology = Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

	{
		Technique line{ Chan::main };
		Step only( "lambertian" );

		auto pvs = VertexShader::Resolve( gfx,"Solid_VS.cso" );
		only.AddBindable( InputLayout::Resolve( gfx,vertices.GetLayout(),*pvs ) );
		only.AddBindable( std::move( pvs ) );

		only.AddBindable( PixelShader::Resolve( gfx,"Solid_PS.cso" ) );

		struct PSColorConstant
		{
			dx::XMFLOAT3 color = { 0.2f,0.2f,0.6f };
			float padding;
		} colorConst;
		only.AddBindable( PixelConstantBuffer<PSColorConstant>::Resolve( gfx,colorConst,1u ) );

		only.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

		only.AddBindable( Rasterizer::Resolve( gfx,false ) );

		line.AddStep( std::move( only ) );
		AddTechnique( std::move( line ) );
	}
}

void CameraIndicator::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void CameraIndicator::SetRotation( DirectX::XMFLOAT3 rot ) noexcept
{
	this->rot = rot;
}

DirectX::XMMATRIX CameraIndicator::GetTransformXM() const noexcept
{
	return dx::XMMatrixRotationRollPitchYawFromVector( dx::XMLoadFloat3( &rot ) ) *
		dx::XMMatrixTranslationFromVector( dx::XMLoadFloat3( &pos ) );
}