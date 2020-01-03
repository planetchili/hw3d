#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>
#include "Technique.h"

class TechniqueProbe;
class Material;
class RenderGraph;
struct aiMesh;

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable( Graphics& gfx,const Material& mat,const aiMesh& mesh,float scale = 1.0f ) noexcept;
	Drawable( const Drawable& ) = delete;
	void AddTechnique( Technique tech_in ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit() const noexcept;
	void Bind( Graphics& gfx ) const noexcept;
	void Accept( TechniqueProbe& probe );
	UINT GetIndexCount() const noxnd;
	void LinkTechniques( RenderGraph& );
	virtual ~Drawable();
protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};