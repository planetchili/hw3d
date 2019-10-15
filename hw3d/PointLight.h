#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"

class PointLight
{
public:
	PointLight( Graphics& gfx,float radius = 0.5f );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit( class FrameCommander& frame ) const noxnd;
	void Bind( Graphics& gfx,DirectX::FXMMATRIX view ) const noexcept;
private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
};