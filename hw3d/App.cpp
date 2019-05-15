#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "SkinnedBox.h"
#include "AssTest.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd( 800,600,"The Donkey Fart Box" ),
	light( wnd.Gfx() )
{
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );

	const auto transform = dx::XMMatrixRotationRollPitchYaw( pos.roll,pos.pitch,pos.yaw ) *
		dx::XMMatrixTranslation( pos.x,pos.y,pos.z );
	nano.Draw( wnd.Gfx(),transform );
	light.Draw( wnd.Gfx() );
		
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if( ImGui::Begin( "Model" ) )
	{
		using namespace std::string_literals;

		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&pos.roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pos.pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&pos.yaw,-180.0f,180.0f );
		
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-20.0f,20.0f );
		ImGui::SliderFloat( "Y",&pos.y,-20.0f,20.0f );
		ImGui::SliderFloat( "Z",&pos.z,-20.0f,20.0f );
	}
	ImGui::End();
}

App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}