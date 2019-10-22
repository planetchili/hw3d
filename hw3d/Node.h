#pragma once
#include "Graphics.h"

class Model;
class Mesh;
class FrameCommander;

class Node
{
	friend Model;
public:
	Node( int id,const std::string& name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform ) noxnd;
	void Submit( FrameCommander& frame,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
	void SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	bool HasChildren() const noexcept
	{
		return childPtrs.size() > 0;
	}
	void Accept( class ModelProbe& probe );
	void Accept( class TechniqueProbe& probe );
	const std::string& GetName() const
	{
		return name;
	}
private:
	void AddChild( std::unique_ptr<Node> pChild ) noxnd;
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};