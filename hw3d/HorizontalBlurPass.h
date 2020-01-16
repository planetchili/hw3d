#pragma once
#include "FullscreenPass.h"
#include "ConstantBuffersEx.h"

class Graphics;
namespace Bind
{
	class PixelShader;
	class RenderTarget;
}

namespace Rgph
{
	class HorizontalBlurPass : public FullscreenPass
	{
	public:
		HorizontalBlurPass( std::string name,Graphics& gfx,unsigned int fullWidth,unsigned int fullHeight );
		void Execute( Graphics& gfx ) const noxnd override;
	private:
		std::shared_ptr<Bind::Bindable> blurScratchIn;
		std::shared_ptr<Bind::Bindable> control;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
	};
}