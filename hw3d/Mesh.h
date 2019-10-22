#pragma once
#include "Graphics.h"
#include "Drawable.h"
#include "ConditionalNoexcept.h"

class Material;
class FrameCommander;
struct aiMesh;


class Mesh : public Drawable
{
public:
	Mesh( Graphics& gfx,const Material& mat,const aiMesh& mesh,float scale = 1.0f ) noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit( FrameCommander& frame,DirectX::FXMMATRIX accumulatedTranform ) const noxnd;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};