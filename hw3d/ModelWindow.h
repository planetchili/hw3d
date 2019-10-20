#pragma once
#include "Graphics.h"
#include "DynamicConstant.h"
#include <optional>
#include <unordered_map>

class Node;

class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show( Graphics& gfx,const char* windowName,const Node& root ) noexcept;
	void ApplyParameters() noxnd;
private:
	DirectX::XMMATRIX GetTransform() const noxnd;
	const Dcb::Buffer& GetMaterial() const noxnd;
	bool TransformDirty() const noxnd;
	void ResetTransformDirty() noxnd;
	bool MaterialDirty() const noxnd;
	void ResetMaterialDirty() noxnd;
	bool IsDirty() const noxnd;
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	struct NodeData
	{
		TransformParameters tranformParams;
		bool transformParamsDirty;
		std::optional<Dcb::Buffer> materialCbuf;
		bool materialCbufDirty;
	};
	std::unordered_map<int,NodeData> transforms;
};
