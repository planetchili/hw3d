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

namespace dx = DirectX;

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280,720,"The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	light( wnd.Gfx() )
{
	TestMaterialSystemLoading( wnd.Gfx() );
	cube.SetPos( { 4.0f,0.0f,0.0f } );
	cube2.SetPos( { 0.0f,4.0f,0.0f } );

	{
		std::string path = "Models\\brick_wall\\brick_wall.obj";
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile( path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);
		Material mat{ wnd.Gfx(),*pScene->mMaterials[1],path };
		pLoaded = std::make_unique<Mesh>( wnd.Gfx(),mat,*pScene->mMeshes[0] );
	}

	//wall.SetRootTransform( dx::XMMatrixTranslation( -12.0f,0.0f,0.0f ) );
	//tp.SetPos( { 12.0f,0.0f,0.0f } );
	//gobber.SetRootTransform( dx::XMMatrixTranslation( 0.0f,0.0f,-4.0f ) );
	//nano.SetRootTransform( dx::XMMatrixTranslation( 0.0f,-7.0f,6.0f ) );
	//bluePlane.SetPos( cam.GetPos() );
	//redPlane.SetPos( cam.GetPos() );

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
	light.Submit( fc );
	//sponza.Draw( wnd.Gfx() );
	//cube.Submit( fc );
	//cube2.Submit( fc );
	pLoaded->Submit( fc,DirectX::XMMatrixIdentity() );
	//bluePlane.Draw( wnd.Gfx() );
	//redPlane.Draw( wnd.Gfx() );
	fc.Execute( wnd.Gfx() );

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
		
	// Mesh techniques window
	class Probe : public TechniqueProbe
	{
	public:
		void OnSetTechnique() override
		{
			using namespace std::string_literals;
			ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f },pTech->GetName().c_str() );
			bool active = pTech->IsActive();
			ImGui::Checkbox( ("Tech Active##"s + std::to_string( techIdx )).c_str(),&active );
			pTech->SetActiveState( active );
		}
		bool OnVisitBuffer( Dcb::Buffer& buf ) override
		{
			namespace dx = DirectX;
			float dirty = false;
			const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{},tagString = "##" + std::to_string( bufIdx )]
			( const char* label ) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if( auto v = buf["scale"]; v.Exists() )
			{
				dcheck( ImGui::SliderFloat( tag( "Scale" ),&v,1.0f,2.0f,"%.3f",3.5f ) );
			}
			if( auto v = buf["materialColor"]; v.Exists() )
			{
				dcheck( ImGui::ColorPicker3( tag( "Color" ),reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v)) ) );
			}
			if( auto v = buf["specularColor"]; v.Exists() )
			{
				dcheck( ImGui::ColorPicker3( tag( "Spec. Color" ),reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v)) ) );
			}
			if( auto v = buf["specularGloss"]; v.Exists() )
			{
				dcheck( ImGui::SliderFloat( tag( "Glossiness" ),&v,1.0f,100.0f,"%.1f",1.5f ) );
			}
			if( auto v = buf["specularWeight"]; v.Exists() )
			{
				dcheck( ImGui::SliderFloat( tag( "Spec. Weight" ),&v,0.0f,2.0f ) );
			}
			if( auto v = buf["useNormalMap"]; v.Exists() )
			{
				dcheck( ImGui::Checkbox( tag( "Normal Map Enable" ),&v ) );
			}
			if( auto v = buf["normalMapWeight"]; v.Exists() )
			{
				dcheck( ImGui::SliderFloat( tag( "Normal Map Weight" ),&v,0.0f,2.0f ) );
			}
			return dirty;
		}
	} probe;
	pLoaded->Accept( probe );
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	cube.SpawnControlWindow( wnd.Gfx(),"Cube 1" );
	cube2.SpawnControlWindow( wnd.Gfx(),"Cube 2" );
	//sponza.ShowWindow( wnd.Gfx(),"Sponza" );
	//gobber.ShowWindow( wnd.Gfx(),"gobber" );
	//wall.ShowWindow( wnd.Gfx(),"Wall" );
	//tp.SpawnControlWindow( wnd.Gfx() );
	//nano.ShowWindow( wnd.Gfx(),"Nano" );
	//bluePlane.SpawnControlWindow( wnd.Gfx(),"Blue Plane" );
	//redPlane.SpawnControlWindow( wnd.Gfx(),"Red Plane" );

	// present
	wnd.Gfx().EndFrame();
	fc.Reset();
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