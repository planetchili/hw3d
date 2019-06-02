#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ConditionalNoexcept.h"

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node( std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform ) noxnd;
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
private:
	void AddChild( std::unique_ptr<Node> pChild ) noxnd;
private:
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model( Graphics& gfx,const std::string fileName );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX transform ) const;
private:
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx,const aiMesh& mesh );
	std::unique_ptr<Node> ParseNode( const aiNode& node );
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};