#include "VertexShader.h"
#include "GraphicsThrowMacros.h"


VertexShader::VertexShader( Graphics& gfx,const std::wstring& path )
{
	INFOMAN( gfx );

	GFX_THROW_INFO( D3DReadFileToBlob( path.c_str(),&pBytecodeBlob ) );
	GFX_THROW_INFO( GetDevice( gfx )->CreateVertexShader( 
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader 
	) );
}

void VertexShader::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->VSSetShader( pVertexShader.Get(),nullptr,0u );
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.Get();
}
