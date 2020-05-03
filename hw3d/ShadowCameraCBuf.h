#pragma once
#include "Bindable.h"
#include "ConstantBuffers.h"

class Camera;

namespace Bind
{
	class ShadowCameraCBuf : public Bindable
	{
	protected:
		struct Transform
		{
			DirectX::XMMATRIX ViewProj;
		};
	public:
		ShadowCameraCBuf( Graphics& gfx,UINT slot = 3u );
		void Bind( Graphics& gfx ) noxnd override;
		void Update( Graphics& gfx );
		void SetCamera( const Camera* pCamera ) noexcept;
	private:
		std::unique_ptr<VertexConstantBuffer<Transform>> pVcbuf;
		const Camera* pCamera = nullptr;
	};
}