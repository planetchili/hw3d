#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Blender.h"
#include "NullPixelShader.h"
#include "ShadowRasterizer.h"
#include "CubeTexture.h"
#include "Viewport.h"
#include "ChiliMath.h"


class Graphics;

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindShadowCamera( const Camera& cam ) noexcept
		{
			pShadowCamera = &cam;
		}
		ShadowMappingPass( Graphics& gfx,std::string name )
			:
			RenderQueuePass( std::move( name ) )
		{
			using namespace Bind;
			depthStencil = std::make_shared<OutputOnlyDepthStencil>( gfx,size,size );
			pDepthCube = std::make_shared<CubeTargetTexture>( gfx,size,size,3,DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT );
			AddBind( VertexShader::Resolve( gfx,"Shadow_VS.cso" ) );
			AddBind( PixelShader::Resolve( gfx,"Shadow_PS.cso" ) );
			AddBind( Stencil::Resolve( gfx,Stencil::Mode::Off ) );
			AddBind( Blender::Resolve( gfx,false ) );
			AddBind( std::make_shared<Viewport>( gfx,(float)size,(float)size ) );
			AddBind( std::make_shared<Bind::Rasterizer>( gfx,false ) );
			RegisterSource( DirectBindableSource<Bind::CubeTargetTexture>::Make( "map",pDepthCube ) );

			DirectX::XMStoreFloat4x4(
				&projection,
				DirectX::XMMatrixPerspectiveFovLH( PI / 2.0f,1.0f,0.5f,100.0f )
			);
			// +x
			DirectX::XMStoreFloat3( &cameraDirections[0],DirectX::XMVectorSet( 1.0f,0.0f,0.0f,0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[0],DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f ) );
			// -x
			DirectX::XMStoreFloat3( &cameraDirections[1],DirectX::XMVectorSet( -1.0f,0.0f,0.0f,0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[1],DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f ) );
			// +y
			DirectX::XMStoreFloat3( &cameraDirections[2],DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[2],DirectX::XMVectorSet( 0.0f,0.0f,-1.0f,0.0f ) );
			// -y
			DirectX::XMStoreFloat3( &cameraDirections[3],DirectX::XMVectorSet( 0.0f,-1.0f,0.0f,0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[3],DirectX::XMVectorSet( 0.0f,0.0f,1.0f,0.0f ) );
			// +z
			DirectX::XMStoreFloat3( &cameraDirections[4],DirectX::XMVectorSet( 0.0f,0.0f,1.0f,0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[4],DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f ) );
			// -z
			DirectX::XMStoreFloat3( &cameraDirections[5],DirectX::XMVectorSet( 0.0f,0.0f,-1.0f,0.0f ) );
			DirectX::XMStoreFloat3( &cameraUps[5],DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f ) );
		}
		void Execute( Graphics& gfx ) const noxnd override
		{
			using namespace DirectX;

			const auto pos = XMLoadFloat3( &pShadowCamera->GetPos() );

			gfx.SetProjection( XMLoadFloat4x4( &projection ) );
			for( size_t i = 0; i < 6; i++ )
			{
				auto rt = pDepthCube->GetRenderTarget( i );
				rt->Clear( gfx );
				depthStencil->Clear( gfx );
				SetRenderTarget( std::move( rt ) );
				const auto lookAt = pos + XMLoadFloat3( &cameraDirections[i] );
				gfx.SetCamera( XMMatrixLookAtLH( pos,lookAt,XMLoadFloat3( &cameraUps[i] ) ) );
				RenderQueuePass::Execute( gfx );
			}
		}
		void DumpShadowMap( Graphics& gfx,const std::string& path ) const
		{
			//for( size_t i = 0; i < 6; i++ )
			//{
			//	auto d = pDepthCube->GetRenderTarget( i );
			//	d->ToSurface( gfx ).Save( path + std::to_string( i ) + ".png" );
			//	d->Dumpy( gfx,path + std::to_string( i ) + ".npy" );
			//}
		}
	private:
		void SetRenderTarget( std::shared_ptr<Bind::RenderTarget> rt ) const
		{
			const_cast<ShadowMappingPass*>(this)->renderTarget = std::move( rt );
		}
		static constexpr UINT size = 1000;
		const Camera* pShadowCamera = nullptr;
		std::shared_ptr<Bind::CubeTargetTexture> pDepthCube;
		DirectX::XMFLOAT4X4 projection;
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraUps{ 6 };
	};
}