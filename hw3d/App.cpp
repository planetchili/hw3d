#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "ChiliUtil.h"
#include "DynamicConstant.h"
#include <cstring>

namespace dx = DirectX;

void TestDynamicConstant()
{
	using namespace std::string_literals;
	// data roundtrip tests
	{
		Dcb::Layout s;
		s.Add<Dcb::Struct>( "butts"s );
		s["butts"s].Add<Dcb::Float3>( "pubes"s );
		s["butts"s].Add<Dcb::Float>( "dank"s );
		s.Add<Dcb::Float>( "woot"s );
		s.Add<Dcb::Array>( "arr"s );
		s["arr"s].Set<Dcb::Struct>( 4 );
		s["arr"s].T().Add<Dcb::Float3>( "twerk"s );
		s["arr"s].T().Add<Dcb::Array>( "werk"s );
		s["arr"s].T()["werk"s].Set<Dcb::Float>( 6 );
		s["arr"s].T().Add<Dcb::Array>( "meta"s );
		s["arr"s].T()["meta"s].Set<Dcb::Array>( 6 );
		s["arr"s].T()["meta"s].T().Set<Dcb::Matrix>( 4 );
		s["arr"s].T().Add<Dcb::Bool>( "booler" );

		// fails: duplicate symbol name
		// s.Add<Dcb::Bool>( "arr"s );

		// fails: bad symbol name
		//s.Add<Dcb::Bool>( "69man" );

		Dcb::Buffer b( s );

		const auto sig = b.GetSignature();


		{
			auto exp = 42.0f;
			b["woot"s] = exp;
			float act = b["woot"s];
			assert( act == exp );
		}
		{
			auto exp = 420.0f;
			b["butts"s]["dank"s] = exp;
			float act = b["butts"s]["dank"s];
			assert( act == exp );
		}
		{
			auto exp = 111.0f;
			b["arr"s][2]["werk"s][5] = exp;
			float act = b["arr"s][2]["werk"s][5];
			assert( act == exp );
		}
		{
			auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
			b["butts"s]["pubes"s] = exp;
			dx::XMFLOAT3 act = b["butts"s]["pubes"s];
			assert( !std::memcmp( &exp,&act,sizeof( DirectX::XMFLOAT3 ) ) );
		}
		{
			DirectX::XMFLOAT4X4 exp;
			dx::XMStoreFloat4x4(
				&exp,
				dx::XMMatrixIdentity()
			);
			b["arr"s][2]["meta"s][5][3] = exp;
			dx::XMFLOAT4X4 act = b["arr"s][2]["meta"s][5][3];
			assert( !std::memcmp( &exp,&act,sizeof( DirectX::XMFLOAT4X4 ) ) );
		}
		{
			auto exp = true;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert( act == exp );
		}
		{
			auto exp = false;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert( act == exp );
		}
		// exists
		{
			assert( b["butts"s]["pubes"s].Exists() );
			assert( !b["butts"s]["fubar"s].Exists() );
			if( auto ref = b["butts"s]["pubes"s]; ref.Exists() )
			{
				dx::XMFLOAT3 f = ref;
				assert( f.x == 69.0f );
			}
		}

		const auto& cb = b;
		{
			dx::XMFLOAT4X4 act = cb["arr"s][2]["meta"s][5][3];
			assert( act._11 == 1.0f );
		}
		// this doesn't compile: buffer is const
		// cb["arr"][2]["booler"] = true;

		// this fails assertion: array out of bounds
		// cb["arr"s][200];

	}
	// size test array of arrays
	{
		Dcb::Layout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Array>( 6 );
		s["arr"].T().Set<Dcb::Matrix>( 4 );
		Dcb::Buffer b( s );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 4u * 4u * 6u );
	}
	// size test array of structs with padding
	{
		Dcb::Layout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Struct>( 6 );
		s["arr"s].T().Add<Dcb::Float2>( "a" );
		s["arr"].T().Add<Dcb::Float3>( "b"s );
		Dcb::Buffer b( s );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 2u * 6u );
	}
	// size test array of primitive that needs padding
	{
		Dcb::Layout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Float3>( 6 );
		Dcb::Buffer b( s );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 6u );
	}
}

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280,720,"The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	light( wnd.Gfx() )
{
	TestDynamicConstant();
	//wall.SetRootTransform( dx::XMMatrixTranslation( -12.0f,0.0f,0.0f ) );
	//tp.SetPos( { 12.0f,0.0f,0.0f } );
	//gobber.SetRootTransform( dx::XMMatrixTranslation( 0.0f,0.0f,-4.0f ) );
	//nano.SetRootTransform( dx::XMMatrixTranslation( 0.0f,-7.0f,6.0f ) );
	bluePlane.SetPos( cam.GetPos() );
	redPlane.SetPos( cam.GetPos() );

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,400.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );
		
	//wall.Draw( wnd.Gfx() );
	//tp.Draw( wnd.Gfx() );
	//nano.Draw( wnd.Gfx() );
	//gobber.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );
	sponza.Draw( wnd.Gfx() );
	bluePlane.Draw( wnd.Gfx() );
	redPlane.Draw( wnd.Gfx() );

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
		
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	//gobber.ShowWindow( wnd.Gfx(),"gobber" );
	//wall.ShowWindow( wnd.Gfx(),"Wall" );
	//tp.SpawnControlWindow( wnd.Gfx() );
	//nano.ShowWindow( wnd.Gfx(),"Nano" );
	sponza.ShowWindow( wnd.Gfx(),"Sponza" );
	bluePlane.SpawnControlWindow( wnd.Gfx(),"Blue Plane" );
	redPlane.SpawnControlWindow( wnd.Gfx(),"Red Plane" );

	// present
	wnd.Gfx().EndFrame();
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
		DoFrame();
	}
}