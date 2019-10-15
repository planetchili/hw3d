#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"

namespace Bind
{
	InputLayout::InputLayout( Graphics& gfx,
		Dvtx::VertexLayout layout_in,
		ID3DBlob* pVertexShaderBytecode )
		:
		layout( std::move( layout_in ) )
	{
		INFOMAN( gfx );

		const auto d3dLayout = layout.GetD3DLayout();

		GFX_THROW_INFO( GetDevice( gfx )->CreateInputLayout(
			d3dLayout.data(),(UINT)d3dLayout.size(),
			pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(),
			&pInputLayout
		) );
	}
	const Dvtx::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return layout;
	}

	void InputLayout::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->IASetInputLayout( pInputLayout.Get() );
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve( Graphics& gfx,
			const Dvtx::VertexLayout& layout,ID3DBlob* pVertexShaderBytecode )
	{
		return Codex::Resolve<InputLayout>( gfx,layout,pVertexShaderBytecode );
	}
	std::string InputLayout::GenerateUID( const Dvtx::VertexLayout& layout,ID3DBlob* pVertexShaderBytecode )
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID( layout );
	}
}
