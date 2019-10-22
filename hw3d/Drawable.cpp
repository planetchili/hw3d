#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
#include <assimp/scene.h>
#include "Material.h"

using namespace Bind;


void Drawable::Submit( FrameCommander& frame ) const noexcept
{
	for( const auto& tech : techniques )
	{
		tech.Submit( frame,*this );
	}
}

Drawable::Drawable( Graphics& gfx,const Material& mat,const aiMesh& mesh,float scale ) noexcept
{
	pVertices = mat.MakeVertexBindable( gfx,mesh,scale );
	pIndices = mat.MakeIndexBindable( gfx,mesh );
	pTopology = Bind::Topology::Resolve( gfx );

	for( auto& t : mat.GetTechniques() )
	{
		AddTechnique( std::move( t ) );
	}
}

void Drawable::AddTechnique( Technique tech_in ) noexcept
{
	tech_in.InitializeParentReferences( *this );
	techniques.push_back( std::move( tech_in ) );
}

void Drawable::Bind( Graphics& gfx ) const noexcept
{
	pTopology->Bind( gfx );
	pIndices->Bind( gfx );
	pVertices->Bind( gfx );
}

void Drawable::Accept( TechniqueProbe& probe )
{
	for( auto& t : techniques )
	{
		t.Accept( probe );
	}
}

UINT Drawable::GetIndexCount() const noxnd
{
	return pIndices->GetCount();
}

Drawable::~Drawable()
{}