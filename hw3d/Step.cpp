#include "Step.h"
#include "Drawable.h"
#include "FrameCommander.h"
#include "TechniqueProbe.h"

void Step::Submit( FrameCommander& frame,const Drawable & drawable ) const
{
	frame.Accept( Job{ this,&drawable },targetPass );
}

void Step::InitializeParentReferences( const Drawable& parent ) noexcept
{
	for( auto& b : bindables )
	{
		b->InitializeParentReference( parent );
	}
}

Step::Step( size_t targetPass_in )
	:
	targetPass{ targetPass_in }
{}

Step::Step( const Step& src ) noexcept
	:
	targetPass( src.targetPass )
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

void Step::Bind( Graphics& gfx ) const
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
