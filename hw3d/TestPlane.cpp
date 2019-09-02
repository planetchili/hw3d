#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "TransformCbufDoubleboi.h"

TestPlane::TestPlane( Graphics& gfx,float size )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform( dx::XMMatrixScaling( size,size,1.0f ) );
	const auto geometryTag = "$plane." + std::to_string( size );
	AddBind( VertexBuffer::Resolve( gfx,geometryTag,model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx,geometryTag,model.indices ) );

	AddBind( Texture::Resolve( gfx,"Images\\brickwall.jpg" ) );
	AddBind( Texture::Resolve( gfx,"Images\\brickwall_normal_obj.png",1u ) );

	auto pvs = VertexShader::Resolve( gfx,"PhongVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx,"PhongPSNormalMapObject.cso" ) );

	AddBind( PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx,pmc,1u ) );

	AddBind( InputLayout::Resolve( gfx,model.vertices.GetLayout(),pvsbc ) );

	AddBind( Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbufDoubleboi>( gfx,*this,0u,2u ) );
}

void TestPlane::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void TestPlane::SetRotation( float roll,float pitch,float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( roll,pitch,yaw ) *
		DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}

void TestPlane::SpawnControlWindow( Graphics& gfx ) noexcept
{
	if( ImGui::Begin( "Plane" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&pos.y,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&pos.z,-80.0f,80.0f,"%.1f" );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		ImGui::Text( "Shading" );
		bool changed0 = ImGui::SliderFloat( "Spec. Int.",&pmc.specularIntensity,0.0f,1.0f );
		bool changed1 = ImGui::SliderFloat( "Spec. Power",&pmc.specularPower,0.0f,100.0f );
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox( "Enable Normal Map",&checkState );
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if( changed0 || changed1 || changed2 )
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx,pmc );
		}
	}
	ImGui::End();
}
