#pragma once

#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"
#include <array>

class Plane
{
public:
	static IndexedTriangleList MakeTesselatedTextured( Dvtx::VertexLayout layout,int divisions_x,int divisions_y )
	{
		namespace dx = DirectX;
		assert( divisions_x >= 1 );
		assert( divisions_y >= 1 );

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int nVertices_x = divisions_x + 1;
		const int nVertices_y = divisions_y + 1;
		Dvtx::VertexBuffer vb{ std::move( layout ) };

		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float divisionSize_x = width / float( divisions_x );
			const float divisionSize_y = height / float( divisions_y );
			const float divisionSize_x_tc = 1.0f / float( divisions_x );
			const float divisionSize_y_tc = 1.0f / float( divisions_y );

			for( int y = 0,i = 0; y < nVertices_y; y++ )
			{
				const float y_pos = float( y ) * divisionSize_y - side_y;
				const float y_pos_tc = 1.0f - float( y ) * divisionSize_y_tc;
				for( int x = 0; x < nVertices_x; x++,i++ )
				{
					const float x_pos = float( x ) * divisionSize_x - side_x;
					const float x_pos_tc = float( x ) * divisionSize_x_tc;
					vb.EmplaceBack(
						dx::XMFLOAT3{ x_pos,y_pos,0.0f },
						dx::XMFLOAT3{ 0.0f,0.0f,-1.0f },
						dx::XMFLOAT2{ x_pos_tc,y_pos_tc }
					);
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve( sq( divisions_x * divisions_y ) * 6 );
		{
			const auto vxy2i = [nVertices_x]( size_t x,size_t y )
			{
				return (unsigned short)(y * nVertices_x + x);
			};
			for( size_t y = 0; y < divisions_y; y++ )
			{
				for( size_t x = 0; x < divisions_x; x++ )
				{
					const std::array<unsigned short,4> indexArray =
					{ vxy2i( x,y ),vxy2i( x + 1,y ),vxy2i( x,y + 1 ),vxy2i( x + 1,y + 1 ) };
					indices.push_back( indexArray[0] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[3] );
				}
			}
		}

		return{ std::move( vb ),std::move( indices ) };
	}
	static IndexedTriangleList Make()
	{
		using Dvtx::VertexLayout;
		VertexLayout vl;
		vl.Append( VertexLayout::Position3D );
		vl.Append( VertexLayout::Normal );
		vl.Append( VertexLayout::Texture2D );

		return MakeTesselatedTextured( std::move( vl ),1,1 );
	}
};
