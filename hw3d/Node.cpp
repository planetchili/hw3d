#include "Node.h"
#include "Mesh.h"
#include "ModelProbe.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Node::Node( int id,const std::string& name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform_in ) noxnd
	:
	id( id ),
	meshPtrs( std::move( meshPtrs ) ),
	name( name )
{
	dx::XMStoreFloat4x4( &transform,transform_in );
	dx::XMStoreFloat4x4( &appliedTransform,dx::XMMatrixIdentity() );
}

void Node::Submit( FrameCommander& frame,DirectX::FXMMATRIX accumulatedTransform ) const noxnd
{
	const auto built =
		dx::XMLoadFloat4x4( &appliedTransform ) *
		dx::XMLoadFloat4x4( &transform ) *
		accumulatedTransform;
	for( const auto pm : meshPtrs )
	{
		pm->Submit( frame,built );
	}
	for( const auto& pc : childPtrs )
	{
		pc->Submit( frame,built );
	}
}

void Node::AddChild( std::unique_ptr<Node> pChild ) noxnd
{
	assert( pChild );
	childPtrs.push_back( std::move( pChild ) );
}

void Node::SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept
{
	dx::XMStoreFloat4x4( &appliedTransform,transform );
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

int Node::GetId() const noexcept
{
	return id;
}

void Node::Accept( ModelProbe& probe )
{
	if( probe.PushNode( *this ) )
	{
		for( auto& cp : childPtrs )
		{
			cp->Accept( probe );
		}
		probe.PopNode( *this );
	}
}

void Node::Accept( TechniqueProbe& probe )
{
	for( auto& mp : meshPtrs )
	{
		mp->Accept( probe );
	}
}
