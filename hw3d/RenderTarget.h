#pragma once
#include "Bindable.h"
#include "BufferResource.h"

class Graphics;

namespace Bind
{
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource
	{
	public:
		void BindAsBuffer( Graphics& gfx ) noexcept override;
		void BindAsBuffer( Graphics& gfx,BufferResource* depthStencil ) noexcept override;
		void BindAsBuffer( Graphics& gfx,DepthStencil* depthStencil ) noexcept;
		void Clear( Graphics& gfx ) noexcept override;
		void Clear( Graphics& gfx,const std::array<float,4>& color ) noexcept;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
	private:
		void BindAsBuffer( Graphics& gfx,ID3D11DepthStencilView* pDepthStencilView ) noexcept;
	protected:
		RenderTarget( Graphics& gfx,ID3D11Texture2D* pTexture );
		RenderTarget( Graphics& gfx,UINT width,UINT height );
		UINT width;
		UINT height;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget( Graphics& gfx,UINT width,UINT height,UINT slot );
		void Bind( Graphics& gfx ) noexcept override;
	private:
		UINT slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};

	// RT for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend Graphics;
	public:
		void Bind( Graphics& gfx ) noexcept override;
	private:
		OutputOnlyRenderTarget( Graphics& gfx,ID3D11Texture2D* pTexture );
	};
}