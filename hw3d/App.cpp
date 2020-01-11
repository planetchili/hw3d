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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "DynamicConstant.h"
#include "ModelProbe.h"
#include "Node.h"
#include "ChiliXM.h"
#include "TechniqueProbe.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"

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

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );
		
	light.Submit();
	cube.Submit();
	sponza.Submit();
	cube2.Submit();

	rg.Execute( wnd.Gfx() );

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
	class TP : public TechniqueProbe
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
			if( auto v = buf["offset"]; v.Exists() )
			{
				dcheck( ImGui::SliderFloat( tag( "offset" ),&v,0.0f,1.0f,"%.3f",2.5f ) );
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
			if( auto v = buf["useSpecularMap"]; v.Exists() )
			{
				dcheck( ImGui::Checkbox( tag( "Spec. Map Enable" ),&v ) );
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
	};

	class MP : ModelProbe
	{
	public:
		void SpawnWindow( Model& model )
		{
			ImGui::Begin( "Model" );
			ImGui::Columns( 2,nullptr,true );
			model.Accept( *this );

			ImGui::NextColumn();
			if( pSelectedNode != nullptr )
			{
				bool dirty = false;
				const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed; };
				auto& tf = ResolveTransform();
				ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f },"Translation" );
				dcheck( ImGui::SliderFloat( "X",&tf.x,-60.f,60.f ) );
				dcheck( ImGui::SliderFloat( "Y",&tf.y,-60.f,60.f ) );
				dcheck( ImGui::SliderFloat( "Z",&tf.z,-60.f,60.f ) );
				ImGui::TextColored( { 0.4f,1.0f,0.6f,1.0f },"Orientation" );
				dcheck( ImGui::SliderAngle( "X-rotation",&tf.xRot,-180.0f,180.0f ) );
				dcheck( ImGui::SliderAngle( "Y-rotation",&tf.yRot,-180.0f,180.0f ) );
				dcheck( ImGui::SliderAngle( "Z-rotation",&tf.zRot,-180.0f,180.0f ) );
				if( dirty )
				{
					pSelectedNode->SetAppliedTransform(
						dx::XMMatrixRotationX( tf.xRot ) *
						dx::XMMatrixRotationY( tf.yRot ) *
						dx::XMMatrixRotationZ( tf.zRot ) *
						dx::XMMatrixTranslation( tf.x,tf.y,tf.z )
					);
				}
			}
			if( pSelectedNode != nullptr )
			{
				TP probe;
				pSelectedNode->Accept( probe );
			}
			ImGui::End();
		}
	protected:
		bool PushNode( Node& node ) override
		{
			// if there is no selected node, set selectedId to an impossible value
			const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
			// build up flags for current node
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			// render this node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetId(),
				node_flags,node.GetName().c_str()
			);
			// processing for selecting node
			if( ImGui::IsItemClicked() )
			{
				// used to change the highlighted node on selection change
				struct Probe : public TechniqueProbe
				{
					virtual void OnSetTechnique()
					{
						if( pTech->GetName() == "Outline" )
						{
							pTech->SetActiveState( highlighted );
						}
					}
					bool highlighted = false;
				} probe;

				// remove highlight on prev-selected node
				if( pSelectedNode != nullptr )
				{
					pSelectedNode->Accept( probe );
				}
				// add highlight to newly-selected node
				probe.highlighted = true;
				node.Accept( probe );

				pSelectedNode = &node;
			}
			// signal if children should also be recursed
			return expanded;
		}
		void PopNode( Node& node ) override
		{
			ImGui::TreePop();
		}
	private:
		Node* pSelectedNode = nullptr;
		struct TransformParameters
		{
			float xRot= 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int,TransformParameters> transformParams;
	private:
		TransformParameters& ResolveTransform() noexcept
		{
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find( id );
			if( i == transformParams.end() )
			{
				return LoadTransform( id );
			}
			return i->second;
		}
		TransformParameters& LoadTransform( int id ) noexcept
		{
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles( applied );
			const auto translation = ExtractTranslation( applied );
			TransformParameters tp;
			tp.zRot = angles.z;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert( { id,{ tp } } ).first->second;
		}
	};
	static MP modelProbe;

	// imgui windows
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
		DoFrame();
	}
}