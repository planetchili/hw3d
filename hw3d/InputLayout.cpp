#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

InputLayout::InputLayout( Graphics& gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
	ID3DBlob* pVertexShaderBytecode )
{
	INFOMAN( gfx );

	GFX_THROW_INFO( GetDevice( gfx )->CreateInputLayout(
		layout.data(),(UINT)layout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	) );
}

void InputLayout::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->IASetInputLayout( pInputLayout.Get() );
}
