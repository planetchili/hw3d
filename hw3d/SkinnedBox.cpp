#include "SkinnedBox.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"


SkinnedBox::SkinnedBox( Graphics& gfx,std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist )
	:
	TestObject( gfx,rng,adist,ddist,odist,rdist )
{
	namespace dx = DirectX;

	if( !IsStaticInitialized() )
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
			dx::XMFLOAT2 tc;
		};
		auto model = Cube::MakeIndependentTextured<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );

		AddStaticBind( std::make_unique<Texture>( gfx,Surface::FromFile( "Images\\kappa50.png" ) ) );

		AddStaticBind( std::make_unique<Sampler>( gfx ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"TexturedPhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"TexturedPhongPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		} colorConst;
		AddStaticBind( std::make_unique<PixelConstantBuffer<PSMaterialConstant>>( gfx,colorConst,1u ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );
}
