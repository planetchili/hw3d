#include "Projection.h"
#include "imgui/imgui.h"

Projection::Projection( float width,float height,float nearZ,float farZ )
	:
	width( width ),
	height( height ),
	nearZ( nearZ ),
	farZ( farZ )
{}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH( width,height,nearZ,farZ );
}

void Projection::RenderWidgets()
{
	ImGui::SliderFloat( "Width",&width,0.01f,4.0f,"%.2f",1.5f );
	ImGui::SliderFloat( "Height",&height,0.01f,4.0f,"%.2f",1.5f );
	ImGui::SliderFloat( "Near Z",&nearZ,0.01f,400.0f,"%.2f",4.0f );
	ImGui::SliderFloat( "Far Z",&farZ,0.01f,400.0f,"%.2f",4.0f );
}
