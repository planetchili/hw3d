#include "CubeTexture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "DepthStencil.h"
#include <vector>

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	CubeTexture::CubeTexture( Graphics& gfx,const std::string& path,UINT slot )
		:
		path( path ),
		slot( slot )
	{
		INFOMAN( gfx );

		// load 6 surfaces for cube faces
		std::vector<Surface> surfaces;
		for( int i = 0; i < 6; i++ )
		{
			surfaces.push_back( Surface::FromFile( path + "\\" + std::to_string( i ) + ".png" ) );
		}

		// texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = surfaces[0].GetWidth();
		textureDesc.Height = surfaces[0].GetHeight();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		// subresource data
		D3D11_SUBRESOURCE_DATA data[6];
		for( int i = 0; i < 6; i++ )
		{
			data[i].pSysMem = surfaces[i].GetBufferPtrConst();
			data[i].SysMemPitch = surfaces[i].GetBytePitch();
			data[i].SysMemSlicePitch = 0;
		}
		// create the texture resource
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D(
			&textureDesc,data,&pTexture
		) );

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView(
			pTexture.Get(),&srvDesc,&pTextureView
		) );
	}

	void CubeTexture::Bind( Graphics& gfx ) noxnd
	{
		INFOMAN_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( slot,1u,pTextureView.GetAddressOf() ) );
	}

	DepthCubeTexture::DepthCubeTexture( Graphics& gfx,UINT size,UINT slot )
		:
		slot( slot )
	{
		INFOMAN( gfx );

		// texture descriptor
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = size;
		textureDesc.Height = size;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		// create the texture resource
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D(
			&textureDesc,nullptr,&pTexture
		) );

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView(
			pTexture.Get(),&srvDesc,&pTextureView
		) );

		// make depth buffer resources for capturing shadow map
		for( UINT face = 0; face < 6; face++ )
		{
			depthBuffers.push_back( std::make_shared<OutputOnlyDepthStencil>( gfx,pTexture,face ) );
		}
	}

	std::shared_ptr<OutputOnlyDepthStencil> Bind::DepthCubeTexture::GetDepthBuffer( size_t index ) const
	{
		return depthBuffers[index];
	}

	void DepthCubeTexture::Bind( Graphics& gfx ) noxnd
	{
		INFOMAN_NOHR( gfx );
		GFX_THROW_INFO_ONLY( GetContext( gfx )->PSSetShaderResources( slot,1u,pTextureView.GetAddressOf() ) );
	}
}
