#pragma once
#include "FullscreenPass.h"
#include "ConstantBuffersEx.h"

class Graphics;
namespace Bind
{
	class PixelShader;
	class RenderTarget;
}

class VerticalBlurPass : public FullscreenPass
{
public:
	VerticalBlurPass( std::string name,Graphics& gfx );
	void Execute( Graphics& gfx ) const noexcept override;
private:
	std::shared_ptr<Bind::Bindable> blurScratchIn;
	std::shared_ptr<Bind::Bindable> control;
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> direction;
};