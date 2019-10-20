#include "ModelWindow.h"
#include <DirectXMath.h>
#include <cassert>
#include "Node.h"

namespace dx = DirectX;


void ModelWindow::Show( Graphics& gfx,const char* windowName,const Node& root ) noexcept
{
	//// window name defaults to "Model"
	//windowName = windowName ? windowName : "Model";
	//// need an ints to track node indices and selected node
	//int nodeIndexTracker = 0;
	//if( ImGui::Begin( windowName ) )
	//{
	//	ImGui::Columns( 2,nullptr,true );
	//	root.ShowTree( pSelectedNode );

	//	ImGui::NextColumn();
	//	if( pSelectedNode != nullptr )
	//	{
	//		const auto id = pSelectedNode->GetId();
	//		auto i = transforms.find( id );
	//		if( i == transforms.end() )
	//		{
	//			const auto& applied = pSelectedNode->GetAppliedTransform();
	//			const auto angles = ExtractEulerAngles( applied );
	//			const auto translation = ExtractTranslation( applied );
	//			TransformParameters tp;
	//			tp.roll = angles.z;
	//			tp.pitch = angles.x;
	//			tp.yaw = angles.y;
	//			tp.x = translation.x;
	//			tp.y = translation.y;
	//			tp.z = translation.z;
	//			auto pMatConst = pSelectedNode->GetMaterialConstants();
	//			auto buf = pMatConst != nullptr ? std::optional<Dcb::Buffer>{ *pMatConst } : std::optional<Dcb::Buffer>{};
	//			std::tie( i,std::ignore ) = transforms.insert( { id,{ tp,false,std::move( buf ),false } } );
	//		}
	//		// link imgui ctrl to our cached transform params
	//		{
	//			auto& transform = i->second.tranformParams;
	//			// dirty check
	//			auto& dirty = i->second.transformParamsDirty;
	//			const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed; };
	//			// widgets
	//			ImGui::Text( "Orientation" );
	//			dcheck( ImGui::SliderAngle( "Roll",&transform.roll,-180.0f,180.0f ) );
	//			dcheck( ImGui::SliderAngle( "Pitch",&transform.pitch,-180.0f,180.0f ) );
	//			dcheck( ImGui::SliderAngle( "Yaw",&transform.yaw,-180.0f,180.0f ) );
	//			ImGui::Text( "Position" );
	//			dcheck( ImGui::SliderFloat( "X",&transform.x,-20.0f,20.0f ) );
	//			dcheck( ImGui::SliderFloat( "Y",&transform.y,-20.0f,20.0f ) );
	//			dcheck( ImGui::SliderFloat( "Z",&transform.z,-20.0f,20.0f ) );
	//		}
	//		// link imgui ctrl to our cached material params
	//		if( i->second.materialCbuf )
	//		{
	//			auto& mat = *i->second.materialCbuf;
	//			// dirty check
	//			auto& dirty = i->second.materialCbufDirty;
	//			const auto dcheck = [&dirty]( bool changed ) {dirty = dirty || changed; };
	//			// widgets
	//			ImGui::Text( "Material" );
	//			if( auto v = mat["normalMapEnabled"]; v.Exists() )
	//			{
	//				dcheck( ImGui::Checkbox( "Norm Map",&v ) );
	//			}
	//			if( auto v = mat["specularMapEnabled"]; v.Exists() )
	//			{
	//				dcheck( ImGui::Checkbox( "Spec Map",&v ) );
	//			}
	//			if( auto v = mat["hasGlossMap"]; v.Exists() )
	//			{
	//				dcheck( ImGui::Checkbox( "Gloss Map",&v ) );
	//			}
	//			if( auto v = mat["materialColor"]; v.Exists() )
	//			{
	//				dcheck( ImGui::ColorPicker3( "Diff Color",reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v)) ) );
	//			}
	//			if( auto v = mat["specularPower"]; v.Exists() )
	//			{
	//				dcheck( ImGui::SliderFloat( "Spec Power",&v,0.0f,100.0f,"%.1f",1.5f ) );
	//			}
	//			if( auto v = mat["specularColor"]; v.Exists() )
	//			{
	//				dcheck( ImGui::ColorPicker3( "Spec Color",reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v)) ) );
	//			}
	//			if( auto v = mat["specularMapWeight"]; v.Exists() )
	//			{
	//				dcheck( ImGui::SliderFloat( "Spec Weight",&v,0.0f,4.0f ) );
	//			}
	//			if( auto v = mat["specularIntensity"]; v.Exists() )
	//			{
	//				dcheck( ImGui::SliderFloat( "Spec Intens",&v,0.0f,1.0f ) );
	//			}
	//		}
	//	}
	//}
	//ImGui::End();
}
void ModelWindow::ApplyParameters() noxnd
{
	//if( TransformDirty() )
	//{
	//	pSelectedNode->SetAppliedTransform( GetTransform() );
	//	ResetTransformDirty();
	//}
	//if( MaterialDirty() )
	//{
	//	pSelectedNode->SetMaterialConstants( GetMaterial() );
	//	ResetMaterialDirty();
	//}
}
dx::XMMATRIX ModelWindow::GetTransform() const noxnd
{
	assert( pSelectedNode != nullptr );
	const auto& transform = transforms.at( pSelectedNode->GetId() ).tranformParams;
	return
		dx::XMMatrixRotationRollPitchYaw( transform.roll,transform.pitch,transform.yaw ) *
		dx::XMMatrixTranslation( transform.x,transform.y,transform.z );
}
const Dcb::Buffer& ModelWindow::GetMaterial() const noxnd
{
	assert( pSelectedNode != nullptr );
	const auto& mat = transforms.at( pSelectedNode->GetId() ).materialCbuf;
	assert( mat );
	return *mat;
}
bool ModelWindow::TransformDirty() const noxnd
{
	return pSelectedNode && transforms.at( pSelectedNode->GetId() ).transformParamsDirty;
}
void ModelWindow::ResetTransformDirty() noxnd
{
	transforms.at( pSelectedNode->GetId() ).transformParamsDirty = false;
}
bool ModelWindow::MaterialDirty() const noxnd
{
	return pSelectedNode && transforms.at( pSelectedNode->GetId() ).materialCbufDirty;
}
void ModelWindow::ResetMaterialDirty() noxnd
{
	transforms.at( pSelectedNode->GetId() ).materialCbufDirty = false;
}
bool ModelWindow::IsDirty() const noxnd
{
	return TransformDirty() || MaterialDirty();
}