#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "DynamicConstant.h"

namespace Bind
{
	class PixelConstantBufferEX : public Bindable
	{
	public:
		PixelConstantBufferEX( Graphics& gfx,const Dcb::LayoutElement& layout,UINT slot )
			:
			PixelConstantBufferEX( gfx,layout,slot,nullptr )
		{}
		PixelConstantBufferEX( Graphics& gfx,const Dcb::Buffer& buf,UINT slot )
			:
			PixelConstantBufferEX( gfx,buf.GetLayout(),slot,&buf )
		{}
		void Update( Graphics& gfx,const Dcb::Buffer& buf )
		{
			INFOMAN( gfx );

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO( GetContext( gfx )->Map(
				pConstantBuffer.Get(),0u,
				D3D11_MAP_WRITE_DISCARD,0u,
				&msr
			) );
			memcpy( msr.pData,buf.GetData(),buf.GetSizeInBytes() );
			GetContext( gfx )->Unmap( pConstantBuffer.Get(),0u );
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->PSSetConstantBuffers( slot,1u,pConstantBuffer.GetAddressOf() );
		}
	private:
		PixelConstantBufferEX( Graphics& gfx,const Dcb::LayoutElement& layout,UINT slot,const Dcb::Buffer* pBuf )
			:
			slot( slot )
		{
			INFOMAN( gfx );

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layout.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if( pBuf != nullptr )
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuf->GetData();
				GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd,&csd,&pConstantBuffer ) );
			}
			else
			{
				GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd,nullptr,&pConstantBuffer ) );
			}
		}
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};
}