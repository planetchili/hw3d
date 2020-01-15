#pragma once
#include "BindingPass.h"

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class InputLayout;
}

class FullscreenPass : public BindingPass
{
public:
	FullscreenPass( const std::string name,Graphics& gfx ) noxnd;
	void Execute( Graphics& gfx ) const noxnd override;
};
