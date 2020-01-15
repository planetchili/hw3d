#include "TransformCbufScaling.h"
#include "TechniqueProbe.h"

namespace dx = DirectX;

Bind::TransformCbufScaling::TransformCbufScaling( Graphics& gfx,float scale )
	:
	TransformCbuf( gfx ),
	buf( MakeLayout() )
{
	buf["scale"] = scale;
}

void Bind::TransformCbufScaling::Accept( TechniqueProbe& probe )
{
	probe.VisitBuffer( buf );
}

void Bind::TransformCbufScaling::Bind( Graphics& gfx ) noxnd
{
	const float scale = buf["scale"];
	const auto scaleMatrix = dx::XMMatrixScaling( scale,scale,scale );
	auto xf = GetTransforms( gfx );
	xf.modelView = xf.modelView * scaleMatrix;
	xf.modelViewProj = xf.modelViewProj * scaleMatrix;
	UpdateBindImpl( gfx,xf );
}

std::unique_ptr<Bind::CloningBindable> Bind::TransformCbufScaling::Clone() const noexcept
{
	return std::make_unique<TransformCbufScaling>( *this );
}

Dcb::RawLayout Bind::TransformCbufScaling::MakeLayout()
{
	Dcb::RawLayout layout;
	layout.Add<Dcb::Float>( "scale" );
	return layout;
}