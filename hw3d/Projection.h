#pragma once
#include <DirectXMath.h>
#include "Frustum.h"

class Graphics;
namespace Rgph
{
	class RenderGraph;
}

class Projection
{
public:
	Projection( Graphics& gfx,float width,float height,float nearZ,float farZ );
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets( Graphics& gfx );
	void SetPos( DirectX::XMFLOAT3 );
	void SetRotation( DirectX::XMFLOAT3 );
	void Submit() const;
	void LinkTechniques( Rgph::RenderGraph& rg );
private:
	float width;
	float height;
	float nearZ;
	float farZ;
	Frustum frust;
};