#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ModelException.h"
#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "ChiliXM.h"

namespace dx = DirectX;

Model::Model( Graphics& gfx,const std::string& pathString,const float scale )
	//:
	//pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( pathString.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if( pScene == nullptr )
	{
		throw ModelException( __LINE__,__FILE__,imp.GetErrorString() );
	}

	// parse materials
	std::vector<Material> materials;
	materials.reserve( pScene->mNumMaterials );
	for( size_t i = 0; i < pScene->mNumMaterials; i++ )
	{
		materials.emplace_back( gfx,*pScene->mMaterials[i],pathString );
	}

	for( size_t i = 0; i < pScene->mNumMeshes; i++ )
	{
		const auto& mesh = *pScene->mMeshes[i];
		meshPtrs.push_back( std::make_unique<Mesh>( gfx,materials[mesh.mMaterialIndex],mesh,scale ) );
	}

	int nextId = 0;
	pRoot = ParseNode( nextId,*pScene->mRootNode,scale );
}

void Model::Submit( FrameCommander& frame ) const noxnd
{
	// I'm still not happy about updating parameters (i.e. mutating a bindable GPU state
	// which is part of a mesh which is part of a node which is part of the model that is
	// const in this call) Can probably do this elsewhere
	//pWindow->ApplyParameters();
	pRoot->Submit( frame,dx::XMMatrixIdentity() );
}

//void Model::ShowWindow( Graphics& gfx,const char* windowName ) noexcept
//{
//	pWindow->Show( gfx,windowName,*pRoot );
//}

void Model::SetRootTransform( DirectX::FXMMATRIX tf ) noexcept
{
	pRoot->SetAppliedTransform( tf );
}

void Model::Accept( ModelProbe & probe )
{
	pRoot->Accept( probe );
}

Model::~Model() noexcept
{}

std::unique_ptr<Node> Model::ParseNode( int& nextId,const aiNode& node,float scale ) noexcept
{
	namespace dx = DirectX;
	const auto transform = ScaleTranslation( dx::XMMatrixTranspose( dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	) ),scale );

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve( node.mNumMeshes );
	for( size_t i = 0; i < node.mNumMeshes; i++ )
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back( meshPtrs.at( meshIdx ).get() );
	}

	auto pNode = std::make_unique<Node>( nextId++,node.mName.C_Str(),std::move( curMeshPtrs ),transform );
	for( size_t i = 0; i < node.mNumChildren; i++ )
	{
		pNode->AddChild( ParseNode( nextId,*node.mChildren[i],scale ) );
	}

	return pNode;
}
