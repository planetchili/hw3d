#pragma once
#include "Graphics.h"
#include <string>
#include <memory>
#include <filesystem>

class Node;
class Mesh;
class FrameCommander;
struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	Model( Graphics& gfx,const std::string& pathString,float scale = 1.0f );
	void Submit( FrameCommander& frame ) const noxnd;
	void ShowWindow( Graphics& gfx,const char* windowName = nullptr ) noexcept;
	void SetRootTransform( DirectX::FXMMATRIX tf ) noexcept;
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx,const aiMesh& mesh,const aiMaterial* const* pMaterials,const std::filesystem::path& path,float scale );
	std::unique_ptr<Node> ParseNode( int& nextId,const aiNode& node ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
};