#include "PixelShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "ChiliUtil.h"

namespace Bind
{
	PixelShader::PixelShader( Graphics& gfx,const std::string& path )
		:
		path( path )
	{
		INFOMAN( gfx );

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO( D3DReadFileToBlob( ToWide( path ).c_str(),&pBlob ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader( pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr,&pPixelShader ) );
	}

	void PixelShader::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShader( pPixelShader.Get(),nullptr,0u );
	}
	std::shared_ptr<PixelShader> PixelShader::Resolve( Graphics& gfx,const std::string& path )
	{
		return Codex::Resolve<PixelShader>( gfx,path );
	}
	std::string PixelShader::GenerateUID( const std::string& path )
	{
		using namespace std::string_literals;
		return typeid(PixelShader).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}
}
