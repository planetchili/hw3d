#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"

class Graphics;
namespace Bind
{
	class Bindable;
	class RenderTarget;
}

class BlurOutlineRenderGraph : public RenderGraph
{
public:
	BlurOutlineRenderGraph( Graphics& gfx );
private:
	// private functions
	void SetKernelGauss( int radius,float sigma ) noxnd;
	// private data
	static constexpr int maxRadius = 7;
	static constexpr int radius = 4;
	static constexpr float sigma = 2.0f;
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurControl;
	std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
};
