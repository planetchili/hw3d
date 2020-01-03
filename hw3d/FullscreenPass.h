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
	void Execute( Graphics& gfx ) const noexcept override;
private:
	static std::shared_ptr<Bind::VertexBuffer> vertexBuffer;
	static std::shared_ptr<Bind::IndexBuffer> indexBuffer;
	static std::shared_ptr<Bind::VertexShader> vertexShader;
	static std::shared_ptr<Bind::InputLayout> inputLayout;
};
