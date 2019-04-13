#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight( Graphics& gfx,float radius )
	:
	mesh( gfx,radius ),
	cbuf( gfx )
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Light" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&cbData.pos.x,-60.0f,60.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&cbData.pos.y,-60.0f,60.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&cbData.pos.z,-60.0f,60.0f,"%.1f" );
		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ 0.0f,0.0f,0.0f },
		{ 0.7f,0.7f,0.9f },
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw( Graphics& gfx ) const noexcept(!IS_DEBUG)
{
	mesh.SetPos( cbData.pos );
	mesh.Draw( gfx );
}

void PointLight::Bind( Graphics& gfx ) const noexcept
{
	cbuf.Update( gfx,cbData );
	cbuf.Bind( gfx );
}
