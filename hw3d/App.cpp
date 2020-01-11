#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "ChiliUtil.h"
#include "Testing.h"
#include "PerfLog.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"
#include "TestModelProbe.h"

namespace dx = DirectX;

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280,720,"The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	light( wnd.Gfx() )
{
	cube.SetPos( { 4.0f,0.0f,0.0f } );
	cube2.SetPos( { 0.0f,4.0f,0.0f } );
	
	{
		{
			auto pass = std::make_unique<BufferClearPass>( "clear" );
			pass->SetInputSource( "renderTarget","$.backbuffer" );
			pass->SetInputSource( "depthStencil","$.masterDepth" );
			rg.AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<LambertianPass>( wnd.Gfx(),"lambertian" );
			pass->SetInputSource( "renderTarget","clear.renderTarget" );
			pass->SetInputSource( "depthStencil","clear.depthStencil" );
			rg.AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>( wnd.Gfx(),"outlineMask" );
			pass->SetInputSource( "depthStencil","lambertian.depthStencil" );
			rg.AppendPass( std::move( pass ) );
		}
		{
			auto pass = std::make_unique<OutlineDrawingPass>( wnd.Gfx(),"outlineDraw" );
			pass->SetInputSource( "renderTarget","lambertian.renderTarget" );
			pass->SetInputSource( "depthStencil","outlineMask.depthStencil" );
			rg.AppendPass( std::move( pass ) );
		}
		rg.SetSinkTarget( "backbuffer","outlineDraw.renderTarget" );
		rg.Finalize();

		cube.LinkTechniques( rg );
		cube2.LinkTechniques( rg );
		light.LinkTechniques( rg );
		sponza.LinkTechniques( rg );
	}

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,400.0f ) );
}

void App::HandleInput( float dt )
{
	while( const auto e = wnd.kbd.ReadKey() )
	{
		if( !e->IsPress() )
		{
			continue;
		}

		switch( e->GetCode() )
		{
		case VK_ESCAPE:
			if( wnd.CursorEnabled() )
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = true;
			break;
		}
	}

	if( !wnd.CursorEnabled() )
	{
		if( wnd.kbd.KeyIsPressed( 'W' ) )
		{
			cam.Translate( { 0.0f,0.0f,dt } );
		}
		if( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cam.Translate( { -dt,0.0f,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cam.Translate( { 0.0f,0.0f,-dt } );
		}
		if( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cam.Translate( { dt,0.0f,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( 'R' ) )
		{
			cam.Translate( { 0.0f,dt,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( 'F' ) )
		{
			cam.Translate( { 0.0f,-dt,0.0f } );
		}
	}

	while( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if( !wnd.CursorEnabled() )
		{
			cam.Rotate( (float)delta->x,(float)delta->y );
		}
	}
}

void App::DoFrame( float dt )
{
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );
		
	light.Submit();
	cube.Submit();
	sponza.Submit();
	cube2.Submit();

	rg.Execute( wnd.Gfx() );
	
	// imgui windows
	static MP modelProbe;
	modelProbe.SpawnWindow( sponza );
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	cube.SpawnControlWindow( wnd.Gfx(),"Cube 1" );
	cube2.SpawnControlWindow( wnd.Gfx(),"Cube 2" );

	// present
	wnd.Gfx().EndFrame();
	rg.Reset();
}

void App::ShowImguiDemoWindow()
{
	if( showDemoWindow )
	{
		ImGui::ShowDemoWindow( &showDemoWindow );
	}
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
		// execute the game logic
		const auto dt = timer.Mark() * speed_factor;
		HandleInput( dt );
		DoFrame( dt );
	}
}