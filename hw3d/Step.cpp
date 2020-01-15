#include "Step.h"
#include "Drawable.h"
#include "RenderGraph.h"
#include "TechniqueProbe.h"
#include "RenderQueuePass.h"

void Step::Submit( const Drawable& drawable ) const
{
	pTargetPass->Accept( Job{ this,&drawable } );
}

void Step::InitializeParentReferences( const Drawable& parent ) noexcept
{
	for( auto& b : bindables )
	{
		b->InitializeParentReference( parent );
	}
}

Step::Step( std::string targetPassName )
	:
	targetPassName{ std::move( targetPassName ) }
{}

Step::Step( const Step& src ) noexcept
	:
	targetPassName( src.targetPassName )
{
	bindables.reserve( src.bindables.size() );
	for( auto& pb : src.bindables )
	{
		if( auto* pCloning = dynamic_cast<const Bind::CloningBindable*>(pb.get()) )
		{
			bindables.push_back( pCloning->Clone() );
		}
		else
		{
			bindables.push_back( pb );
		}
	}
}

void Step::AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept
{
	bindables.push_back( std::move( bind_in ) );
}

void Step::Bind( Graphics& gfx ) const noxnd
{
	for( const auto& b : bindables )
	{
		b->Bind( gfx );
	}
}

void Step::Accept( TechniqueProbe& probe )
{
	probe.SetStep( this );
	for( auto& pb : bindables )
	{
		pb->Accept( probe );
	}
}

void Step::Link( RenderGraph& rg )
{
	assert( pTargetPass == nullptr );
	pTargetPass = &rg.GetRenderQueue( targetPassName );
}
