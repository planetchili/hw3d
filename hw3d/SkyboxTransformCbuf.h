#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class Camera;

namespace Bind
{
	class SkyboxTransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX viewProj;
		};
	public:
		SkyboxTransformCbuf( Graphics& gfx,UINT slot = 0u );
		void Bind( Graphics& gfx ) noxnd override;
	protected:
		void UpdateBindImpl( Graphics& gfx,const Transforms& tf ) noxnd;
		Transforms GetTransforms( Graphics& gfx ) noxnd;
	private:
		std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	};
}