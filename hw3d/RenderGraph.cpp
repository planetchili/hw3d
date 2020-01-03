#include "RenderGraph.h"
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "BindableCommon.h"
#include "RenderGraphCompileException.h"
#include "RenderQueuePass.h"
#include "PassInput.h"
#include "PassOutput.h"
#include <sstream>

RenderGraph::RenderGraph( Graphics& gfx )
	:
	backBufferTarget( gfx.GetTarget() ),
	masterDepth( std::make_shared<Bind::OutputOnlyDepthStencil>( gfx ) )
{
	// setup global sinks and sources
	globalSources.push_back( BufferOutput<Bind::RenderTarget>::Make( "backbuffer",backBufferTarget ) );
	globalSources.push_back( BufferOutput<Bind::DepthStencil>::Make( "masterDepth",masterDepth ) );
	globalSinks.push_back( BufferInput<Bind::RenderTarget>::Make( "backbuffer",backBufferTarget ) );
}

RenderGraph::~RenderGraph()
{}

void RenderGraph::SetSinkTarget( const std::string& sinkName,const std::string& target )
{
	const auto finder = [&sinkName]( const std::unique_ptr<PassInput>& p ) {
		return p->GetRegisteredName() == sinkName;
	};
	const auto i = std::find_if( globalSinks.begin(),globalSinks.end(),finder );
	if( i == globalSinks.end() )
	{
		throw RGC_EXCEPTION( "Global sink does not exist: " + sinkName );
	}
	auto targetSplit = SplitString( target,"." );
	if( targetSplit.size() != 2u )
	{
		throw RGC_EXCEPTION( "Input target has incorrect format" );
	}
	(*i)->SetTarget( targetSplit[0],targetSplit[1] );
}

void RenderGraph::Execute( Graphics& gfx ) noxnd
{
	assert( finalized );
	for( auto& p : passes )
	{
		p->Execute( gfx );
	}
}

void RenderGraph::Reset() noexcept
{
	assert( finalized );
	for( auto& p : passes )
	{
		p->Reset();
	}
}

void RenderGraph::AppendPass( std::unique_ptr<Pass> pass )
{
	assert( !finalized );
	// validate name uniqueness
	for( const auto& p : passes )
	{
		if( pass->GetName() == p->GetName() )
		{
			throw RGC_EXCEPTION( "Pass name already exists: " + pass->GetName() );
		}
	}

	// link outputs from passes (and global outputs) to pass inputs
	LinkPassInputs( *pass );

	// add to container of passes
	passes.push_back( std::move( pass ) );
}

void RenderGraph::LinkPassInputs( Pass& pass )
{
	for( auto& in : pass.GetInputs() )
	{
		const auto& inputSourcePassName = in->GetPassName();

		// check check whether target source is global
		if( inputSourcePassName == "$" )
		{
			bool bound = false;
			for( auto& source : globalSources )
			{
				if( source->GetName() == in->GetOutputName() )
				{
					in->Bind( *source );
					bound = true;
					break;
				}
			}
			if( !bound )
			{
				std::ostringstream oss;
				oss << "Output named [" << in->GetOutputName() << "] not found in globals";
				throw RGC_EXCEPTION( oss.str() );
			}
		}
		else // find source from within existing passes
		{
			for( auto& existingPass : passes )
			{
				if( existingPass->GetName() == inputSourcePassName )
				{
					auto& source = existingPass->GetOutput( in->GetOutputName() );
					in->Bind( source );
					break;
				}
			}
		}
	}
}

void RenderGraph::LinkGlobalSinks()
{
	for( auto& sink : globalSinks )
	{
		const auto& inputSourcePassName = sink->GetPassName();
		for( auto& existingPass : passes )
		{
			if( existingPass->GetName() == inputSourcePassName )
			{
				auto& source = existingPass->GetOutput( sink->GetOutputName() );
				sink->Bind( source );
				break;
			}
		}
	}
}

void RenderGraph::Finalize()
{
	assert( !finalized );
	for( const auto& p : passes )
	{
		p->Finalize();
	}
	LinkGlobalSinks();
	finalized = true;
}

RenderQueuePass& RenderGraph::GetRenderQueue( const std::string& passName )
{
	try
	{
		for( const auto& p : passes )
		{
			if( p->GetName() == passName )
			{
				return dynamic_cast<RenderQueuePass&>(*p);
			}
		}
	}
	catch( std::bad_cast& )
	{
		throw RGC_EXCEPTION( "In RenderGraph::GetRenderQueue, pass was not RenderQueuePass: " + passName );
	}
	throw RGC_EXCEPTION( "In RenderGraph::GetRenderQueue, pass not found: " + passName );
}