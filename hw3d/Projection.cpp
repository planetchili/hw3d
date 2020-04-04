#include "Projection.h"
#include "imgui/imgui.h"
#include "Graphics.h"

Projection::Projection( Graphics& gfx,float width,float height,float nearZ,float farZ )
	:
	width( width ),
	height( height ),
	nearZ( nearZ ),
	farZ( farZ ),
	frust( gfx,width,height,nearZ,farZ ),
	homeWidth( width ),homeHeight( height ),homeNearZ( nearZ ),homeFarZ( farZ )
{}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH( width,height,nearZ,farZ );
}

void Projection::RenderWidgets( Graphics& gfx )
{
	bool dirty = false;
	const auto dcheck = [&dirty]( bool d ) { dirty = dirty || d; };

	ImGui::Text( "Projection" );
	dcheck( ImGui::SliderFloat( "Width",&width,0.01f,4.0f,"%.2f",1.5f ) );
	dcheck( ImGui::SliderFloat( "Height",&height,0.01f,4.0f,"%.2f",1.5f ) );
	dcheck( ImGui::SliderFloat( "Near Z",&nearZ,0.01f,farZ - 0.01f,"%.2f",4.0f ) );
	dcheck( ImGui::SliderFloat( "Far Z",&farZ,nearZ + 0.01f,400.0f,"%.2f",4.0f ) );

	if( dirty )
	{
		frust.SetVertices( gfx,width,height,nearZ,farZ );
	}
}

void Projection::SetPos( DirectX::XMFLOAT3 pos )
{
	frust.SetPos( pos );
}

void Projection::SetRotation( DirectX::XMFLOAT3 rot )
{
	frust.SetRotation( rot );
}

void Projection::Submit() const
{
	frust.Submit();
}

void Projection::LinkTechniques( Rgph::RenderGraph& rg )
{
	frust.LinkTechniques( rg );
}

void Projection::Reset( Graphics& gfx )
{
	width = homeWidth;
	height = homeHeight;
	nearZ = homeNearZ;
	farZ = homeFarZ;
	frust.SetVertices( gfx,width,height,nearZ,farZ );	
}
