#pragma once
#include <DirectXMath.h>

class Projection
{
public:
	Projection( float width,float height,float nearZ,float farZ );
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets();
private:
	float width;
	float height;
	float nearZ;
	float farZ;
};