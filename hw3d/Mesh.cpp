#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>

namespace dx = DirectX;

ModelException::ModelException( int line,const char* file,std::string note ) noexcept
	:
	ChiliException( line,file ),
	note( std::move( note ) )
{}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Chili Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

// Mesh
Mesh::Mesh( Graphics& gfx,std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs )
{
	AddBind( Bind::Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	for( auto& pb : bindPtrs )
	{
		AddBind( std::move( pb ) );
	}

	AddBind( std::make_shared<Bind::TransformCbuf>( gfx,*this ) );
}
void Mesh::Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd
{
	DirectX::XMStoreFloat4x4( &transform,accumulatedTransform );
	Drawable::Draw( gfx );
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4( &transform );
}


// Node
Node::Node( int id,const std::string& name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform_in ) noxnd
	:
	id( id ),
	meshPtrs( std::move( meshPtrs ) ),
	name( name )
{
	dx::XMStoreFloat4x4( &transform,transform_in );
	dx::XMStoreFloat4x4( &appliedTransform,dx::XMMatrixIdentity() );
}

void Node::Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd
{
	const auto built =
		dx::XMLoadFloat4x4( &appliedTransform ) *
		dx::XMLoadFloat4x4( &transform ) * 
		accumulatedTransform;
	for( const auto pm : meshPtrs )
	{
		pm->Draw( gfx,built );
	}
	for( const auto& pc : childPtrs )
	{
		pc->Draw( gfx,built );
	}
}

void Node::AddChild( std::unique_ptr<Node> pChild ) noxnd
{
	assert( pChild );
	childPtrs.push_back( std::move( pChild ) );
}

void Node::ShowTree( Node*& pSelectedNode ) const noexcept
{
	// if there is no selected node, set selectedId to an impossible value
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	// render this node
	const auto expanded = ImGui::TreeNodeEx( 
		(void*)(intptr_t)GetId(),node_flags,name.c_str()
	);
	// processing for selecting node
	if( ImGui::IsItemClicked() )
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	// recursive rendering of open node's children
	if( expanded )
	{
		for( const auto& pChild : childPtrs )
		{
			pChild->ShowTree( pSelectedNode );
		}
		ImGui::TreePop();
	}
}

void Node::SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept
{
	dx::XMStoreFloat4x4( &appliedTransform,transform );
}

int Node::GetId() const noexcept
{
	return id;
}


// Model
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show( const char* windowName,const Node& root ) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if( ImGui::Begin( windowName ) )
		{
			ImGui::Columns( 2,nullptr,true );
			root.ShowTree( pSelectedNode );

			ImGui::NextColumn();
			if( pSelectedNode != nullptr )
			{
				auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text( "Orientation" );
				ImGui::SliderAngle( "Roll",&transform.roll,-180.0f,180.0f );
				ImGui::SliderAngle( "Pitch",&transform.pitch,-180.0f,180.0f );
				ImGui::SliderAngle( "Yaw",&transform.yaw,-180.0f,180.0f );
				ImGui::Text( "Position" );
				ImGui::SliderFloat( "X",&transform.x,-20.0f,20.0f );
				ImGui::SliderFloat( "Y",&transform.y,-20.0f,20.0f );
				ImGui::SliderFloat( "Z",&transform.z,-20.0f,20.0f );
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		assert( pSelectedNode != nullptr );
		const auto& transform = transforms.at( pSelectedNode->GetId() );
		return 
			dx::XMMatrixRotationRollPitchYaw( transform.roll,transform.pitch,transform.yaw ) *
			dx::XMMatrixTranslation( transform.x,transform.y,transform.z );
	}
	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
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
	std::unordered_map<int,TransformParameters> transforms;
};

Model::Model( Graphics& gfx,const std::string fileName )
	:
	pWindow( std::make_unique<ModelWindow>() )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( fileName.c_str(),
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

	for( size_t i = 0; i < pScene->mNumMeshes; i++ )
	{
		meshPtrs.push_back( ParseMesh( gfx,*pScene->mMeshes[i],pScene->mMaterials ) );
	}

	int nextId = 0;
	pRoot = ParseNode( nextId,*pScene->mRootNode );
}

void Model::Draw( Graphics& gfx ) const noxnd
{
	if( auto node = pWindow->GetSelectedNode() )
	{
		node->SetAppliedTransform( pWindow->GetTransform() );
	}
	pRoot->Draw( gfx,dx::XMMatrixIdentity() );
}

void Model::ShowWindow( const char* windowName ) noexcept
{
	pWindow->Show( windowName,*pRoot );
}

void Model::SetRootTransform( DirectX::FXMMATRIX tf ) noexcept
{
	pRoot->SetAppliedTransform( tf );
}

Model::~Model() noexcept
{}

std::unique_ptr<Mesh> Model::ParseMesh( Graphics& gfx,const aiMesh& mesh,const aiMaterial* const* pMaterials )
{
	using namespace std::string_literals;
	using Dvtx::VertexLayout;
	using namespace Bind;

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	const auto base = "Models\\gobber\\"s;

	bool hasSpecularMap = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	float shininess = 35.0f;
	if( mesh.mMaterialIndex >= 0 )
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];

		aiString texFileName;

		if( material.GetTexture( aiTextureType_DIFFUSE,0,&texFileName ) == aiReturn_SUCCESS )
		{
			bindablePtrs.push_back( Texture::Resolve( gfx,base + texFileName.C_Str() ) );
			hasDiffuseMap = true;
		}

		if( material.GetTexture( aiTextureType_SPECULAR,0,&texFileName ) == aiReturn_SUCCESS )
		{
			bindablePtrs.push_back( Texture::Resolve( gfx,base + texFileName.C_Str(),1 ) );
			hasSpecularMap = true;
		}
		else
		{
			material.Get( AI_MATKEY_SHININESS,shininess );
		}

		if( material.GetTexture( aiTextureType_NORMALS,0,&texFileName ) == aiReturn_SUCCESS )
		{
			bindablePtrs.push_back( Texture::Resolve( gfx,base + texFileName.C_Str(),2 ) );
			hasNormalMap = true;
		}

		if( hasDiffuseMap || hasSpecularMap || hasNormalMap )
		{
			bindablePtrs.push_back( Bind::Sampler::Resolve( gfx ) );
		}
	}

	const auto meshTag = base + "%" + mesh.mName.C_Str();

	const float scale = 6.0f;

	if( hasDiffuseMap && hasNormalMap && hasSpecularMap )
	{
		Dvtx::VertexBuffer vbuf( std::move(
			VertexLayout{}
			.Append( VertexLayout::Position3D )
			.Append( VertexLayout::Normal )
			.Append( VertexLayout::Tangent )
			.Append( VertexLayout::Bitangent )
			.Append( VertexLayout::Texture2D )
		) );

		for( unsigned int i = 0; i < mesh.mNumVertices; i++ )
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3( mesh.mVertices[i].x * scale,mesh.mVertices[i].y * scale,mesh.mVertices[i].z * scale ),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve( mesh.mNumFaces * 3 );
		for( unsigned int i = 0; i < mesh.mNumFaces; i++ )
		{
			const auto& face = mesh.mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}

		bindablePtrs.push_back( VertexBuffer::Resolve( gfx,meshTag,vbuf ) );

		bindablePtrs.push_back( IndexBuffer::Resolve( gfx,meshTag,indices ) );

		auto pvs = VertexShader::Resolve( gfx,"PhongVSNormalMap.cso" );
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back( std::move( pvs ) );

		bindablePtrs.push_back( PixelShader::Resolve( gfx,"PhongPSSpecNormalMap.cso" ) );

		bindablePtrs.push_back( InputLayout::Resolve( gfx,vbuf.GetLayout(),pvsbc ) );

		struct PSMaterialConstantFullmonte
		{
			BOOL  normalMapEnabled = TRUE;
			float padding[3];
		} pmc;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back( PixelConstantBuffer<PSMaterialConstantFullmonte>::Resolve( gfx,pmc,1u ) );
	}
	else if( hasDiffuseMap && hasNormalMap )
	{
		Dvtx::VertexBuffer vbuf( std::move(
			VertexLayout{}
			.Append( VertexLayout::Position3D )
			.Append( VertexLayout::Normal )
			.Append( VertexLayout::Tangent )
			.Append( VertexLayout::Bitangent )
			.Append( VertexLayout::Texture2D )
		) );

		for( unsigned int i = 0; i < mesh.mNumVertices; i++ )
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3( mesh.mVertices[i].x * scale,mesh.mVertices[i].y * scale,mesh.mVertices[i].z * scale ),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve( mesh.mNumFaces * 3 );
		for( unsigned int i = 0; i < mesh.mNumFaces; i++ )
		{
			const auto& face = mesh.mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}

		bindablePtrs.push_back( VertexBuffer::Resolve( gfx,meshTag,vbuf ) );

		bindablePtrs.push_back( IndexBuffer::Resolve( gfx,meshTag,indices ) );

		auto pvs = VertexShader::Resolve( gfx,"PhongVSNormalMap.cso" );
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back( std::move( pvs ) );

		bindablePtrs.push_back( PixelShader::Resolve( gfx,"PhongPSNormalMap.cso" ) );

		bindablePtrs.push_back( InputLayout::Resolve( gfx,vbuf.GetLayout(),pvsbc ) );

		struct PSMaterialConstantDiffnorm
		{
			float specularIntensity = 0.18f;
			float specularPower;
			BOOL  normalMapEnabled = TRUE;
			float padding[1];
		} pmc;
		pmc.specularPower = shininess;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back( PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve( gfx,pmc,1u ) );
	}
	else if( hasDiffuseMap )
	{
		Dvtx::VertexBuffer vbuf( std::move(
			VertexLayout{}
			.Append( VertexLayout::Position3D )
			.Append( VertexLayout::Normal )
			.Append( VertexLayout::Texture2D )
		) );

		for( unsigned int i = 0; i < mesh.mNumVertices; i++ )
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3( mesh.mVertices[i].x * scale,mesh.mVertices[i].y * scale,mesh.mVertices[i].z * scale ),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve( mesh.mNumFaces * 3 );
		for( unsigned int i = 0; i < mesh.mNumFaces; i++ )
		{
			const auto& face = mesh.mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}

		bindablePtrs.push_back( VertexBuffer::Resolve( gfx,meshTag,vbuf ) );

		bindablePtrs.push_back( IndexBuffer::Resolve( gfx,meshTag,indices ) );

		auto pvs = VertexShader::Resolve( gfx,"PhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back( std::move( pvs ) );

		bindablePtrs.push_back( PixelShader::Resolve( gfx,"PhongPS.cso" ) );

		bindablePtrs.push_back( InputLayout::Resolve( gfx,vbuf.GetLayout(),pvsbc ) );

		struct PSMaterialConstantDiffuse
		{
			float specularIntensity = 0.18f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back( PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve( gfx,pmc,1u ) );
	}
	else if( !hasDiffuseMap && !hasNormalMap && !hasSpecularMap )
	{
		Dvtx::VertexBuffer vbuf( std::move(
			VertexLayout{}
			.Append( VertexLayout::Position3D )
			.Append( VertexLayout::Normal )
		) );

		for( unsigned int i = 0; i < mesh.mNumVertices; i++ )
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3( mesh.mVertices[i].x * scale,mesh.mVertices[i].y * scale,mesh.mVertices[i].z * scale ),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve( mesh.mNumFaces * 3 );
		for( unsigned int i = 0; i < mesh.mNumFaces; i++ )
		{
			const auto& face = mesh.mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}

		bindablePtrs.push_back( VertexBuffer::Resolve( gfx,meshTag,vbuf ) );

		bindablePtrs.push_back( IndexBuffer::Resolve( gfx,meshTag,indices ) );

		auto pvs = VertexShader::Resolve( gfx,"PhongVSNotex.cso" );
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back( std::move( pvs ) );

		bindablePtrs.push_back( PixelShader::Resolve( gfx,"PhongPSNotex.cso" ) );

		bindablePtrs.push_back( InputLayout::Resolve( gfx,vbuf.GetLayout(),pvsbc ) );

		struct PSMaterialConstantNotex
		{
			dx::XMFLOAT4 materialColor = { 0.65f,0.65f,0.85f,1.0f };
			float specularIntensity = 0.18f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back( PixelConstantBuffer<PSMaterialConstantNotex>::Resolve( gfx,pmc,1u ) );
	}
	else
	{
		throw std::runtime_error( "terrible combination of textures in material smh" );
	}

	return std::make_unique<Mesh>( gfx,std::move( bindablePtrs ) );
}

std::unique_ptr<Node> Model::ParseNode( int& nextId,const aiNode& node ) noexcept
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose( dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	) );

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
		pNode->AddChild( ParseNode( nextId,*node.mChildren[i] ) );
	}

	return pNode;
}
