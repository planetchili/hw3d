#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class OutputOnlyDepthStencil;

	class CubeTexture : public Bindable
	{
	public:
		CubeTexture( Graphics& gfx,const std::string& path,UINT slot = 0 );
		void Bind( Graphics& gfx ) noxnd override;
	private:
		unsigned int slot;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};

	class DepthCubeTexture : public Bindable
	{
	public:
		DepthCubeTexture( Graphics& gfx,UINT size,UINT slot = 0 );
		void Bind( Graphics& gfx ) noxnd override;
		std::shared_ptr<OutputOnlyDepthStencil> GetDepthBuffer( size_t index ) const;
	private:
		unsigned int slot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::vector<std::shared_ptr<OutputOnlyDepthStencil>> depthBuffers;
	};
}
