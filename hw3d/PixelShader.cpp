#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

PixelShader::PixelShader( Graphics& gfx,const std::wstring& path )
{
	INFOMAN( gfx );

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO( D3DReadFileToBlob( path.c_str(),&pBlob ) );
	GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader( pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr,&pPixelShader ) );
}

void PixelShader::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->PSSetShader( pPixelShader.Get(),nullptr,0u );
}
