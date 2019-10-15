#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCommon.h"
#include "BindableCodex.h"

using namespace Bind;


void Drawable::Submit( FrameCommander& frame ) const noexcept
{
	for( const auto& tech : techniques )
	{
		tech.Submit( frame,*this );
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

UINT Drawable::GetIndexCount() const noxnd
{
	return pIndices->GetCount();
}

Drawable::~Drawable()
{}