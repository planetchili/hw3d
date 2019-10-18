#pragma once
#include <assimp/scene.h>
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include "Technique.h"

class Material
{
public:
	Material( Graphics& gfx,const aiMaterial* pMaterial,const std::filesystem::path& path ) noxnd
	{

	}
	Dvtx::VertexBuffer ExtractVertices( const aiMesh& mesh ) const noexcept
	{
		using Type = Dvtx::VertexLayout::ElementType;
		Dvtx::VertexBuffer buf{ vtxLayout };
		buf.Resize( mesh.mNumVertices );
		if( vtxLayout.Has<Type::Position3D>() )
		{
			for( int i = 0; i < mesh.mNumVertices; i++ )
			{
				buf[i].
			}
		}
	}
	std::vector<Technique> GetTechniques() const noexcept
	{
		return techniques;
	}
private:
	Dvtx::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
};