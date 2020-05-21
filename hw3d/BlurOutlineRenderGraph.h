#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"

class Graphics;
class Camera;
namespace Bind
{
	class Bindable;
	class RenderTarget;
}

namespace Rgph
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph( Graphics& gfx );
		void RenderWindows( Graphics& gfx );
		void DumpShadowMap( Graphics& gfx,const std::string& path );
		void BindMainCamera( Camera& cam );
		void BindShadowCamera( Camera& cam );
	private:
		void RenderKernelWindow( Graphics& gfx );
		void RenderShadowWindow( Graphics& gfx );
		// private functions
		void SetKernelGauss( int radius,float sigma ) noxnd;
		void SetKernelBox( int radius ) noxnd;
		// private data
		enum class KernelType
		{
			Gauss,
			Box,
		} kernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> shadowControl;
	};
}
