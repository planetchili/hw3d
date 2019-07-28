#pragma once
#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Sphere
{
public:
	static IndexedTriangleList MakeTesselated( Dvtx::VertexLayout layout,int latDiv,int longDiv )
	{
		namespace dx = DirectX;
		assert( latDiv >= 3 );
		assert( longDiv >= 3 );

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet( 0.0f,0.0f,radius,0.0f );
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;

		Dvtx::VertexBuffer vb{ std::move( layout ) };
		for( int iLat = 1; iLat < latDiv; iLat++ )
		{
			const auto latBase = dx::XMVector3Transform( 
				base,
				dx::XMMatrixRotationX( lattitudeAngle * iLat ) 
			);
			for( int iLong = 0; iLong < longDiv; iLong++ )
			{
				dx::XMFLOAT3 calculatedPos;
				auto v = dx::XMVector3Transform( 
					latBase,
					dx::XMMatrixRotationZ( longitudeAngle * iLong )
				);
				dx::XMStoreFloat3( &calculatedPos,v );
				vb.EmplaceBack( calculatedPos );
			}
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 northPos;
			dx::XMStoreFloat3( &northPos,base );
			vb.EmplaceBack( northPos );
		}
		const auto iSouthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 southPos;
			dx::XMStoreFloat3( &southPos,dx::XMVectorNegate( base ) );
			vb.EmplaceBack( southPos );
		}
		
		const auto calcIdx = [latDiv,longDiv]( unsigned short iLat,unsigned short iLong )
			{ return iLat * longDiv + iLong; };
		std::vector<unsigned short> indices;
		for( unsigned short iLat = 0; iLat < latDiv - 2; iLat++ )
		{
			for( unsigned short iLong = 0; iLong < longDiv - 1; iLong++ )
			{
				indices.push_back( calcIdx( iLat,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong + 1 ) );
			}
			// wrap band
			indices.push_back( calcIdx( iLat,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat,0 ) );
			indices.push_back( calcIdx( iLat,0 ) );
			indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat + 1,0 ) );			
		}

		// cap fans
		for( unsigned short iLong = 0; iLong < longDiv - 1; iLong++ )
		{
			// north
			indices.push_back( iNorthPole );
			indices.push_back( calcIdx( 0,iLong ) );
			indices.push_back( calcIdx( 0,iLong + 1 ) );
			// south
			indices.push_back( calcIdx( latDiv - 2,iLong + 1 ) );
			indices.push_back( calcIdx( latDiv - 2,iLong ) );
			indices.push_back( iSouthPole );
		}
		// wrap triangles
		// north
		indices.push_back( iNorthPole );
		indices.push_back( calcIdx( 0,longDiv - 1 ) );
		indices.push_back( calcIdx( 0,0 ) );
		// south
		indices.push_back( calcIdx( latDiv - 2,0 ) );
		indices.push_back( calcIdx( latDiv - 2,longDiv - 1 ) );
		indices.push_back( iSouthPole );

		return { std::move( vb ),std::move( indices ) };
	}
	static IndexedTriangleList Make( std::optional<Dvtx::VertexLayout> layout = std::nullopt )
	{
		using Element = Dvtx::VertexLayout::ElementType;
		if( !layout )
		{
			layout = Dvtx::VertexLayout{}.Append( Element::Position3D );
		}
		return MakeTesselated( std::move( *layout ),12,24 );
	}
};