#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
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
}
