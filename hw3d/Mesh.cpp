#include "Mesh.h"
#include "imgui/imgui.h"

// Mesh
Mesh::Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs )
{
	if( !IsStaticInitialized() )
	{
		AddStaticBind( std::make_unique<Bind::Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}

	for( auto& pb : bindPtrs )
	{
		if( auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()) )
		{
			AddIndexBuffer( std::unique_ptr<Bind::IndexBuffer>{ pi } );
			pb.release();
		}
		else
		{
			AddBind( std::move( pb ) );
		}
	}

	AddBind( std::make_unique<Bind::TransformCbuf>( gfx,*this ) );
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
Node::Node( const std::string& name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform ) noxnd
	:
	meshPtrs( std::move( meshPtrs ) ),
	name( name )
{
	DirectX::XMStoreFloat4x4( &this->transform,transform );
}

void Node::Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd
{
	const auto built = DirectX::XMLoadFloat4x4( &transform ) * accumulatedTransform;
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

void Node::RenderTree() const noexcept
{
	// if tree node expanded, recursively render all children
	if( ImGui::TreeNode( name.c_str() ) )
	{
		for( const auto& pChild : childPtrs )
		{
			pChild->RenderTree();
		}
		ImGui::TreePop();
	}
}


// Model
Model::Model( Graphics& gfx,const std::string fileName )
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	for( size_t i = 0; i < pScene->mNumMeshes; i++ )
	{
		meshPtrs.push_back( ParseMesh( gfx,*pScene->mMeshes[i] ) );
	}

	pRoot = ParseNode( *pScene->mRootNode );
}

void Model::Draw( Graphics& gfx ) const noxnd
{
	const auto transform = DirectX::XMMatrixRotationRollPitchYaw( pos.roll,pos.pitch,pos.yaw ) *
		DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
	pRoot->Draw( gfx,transform );
}

void Model::ShowWindow( const char* windowName ) noexcept
{
	windowName = windowName ? windowName : "Model";
	if( ImGui::Begin( windowName ) )
	{
		ImGui::Columns( 2,nullptr,true );
		pRoot->RenderTree();

		ImGui::NextColumn();
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&pos.roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pos.pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&pos.yaw,-180.0f,180.0f );
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-20.0f,20.0f );
		ImGui::SliderFloat( "Y",&pos.y,-20.0f,20.0f );
		ImGui::SliderFloat( "Z",&pos.z,-20.0f,20.0f );
	}
	ImGui::End();
}

std::unique_ptr<Mesh> Model::ParseMesh( Graphics& gfx,const aiMesh& mesh )
{
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

	Dvtx::VertexBuffer vbuf( std::move(
		VertexLayout{}
		.Append( VertexLayout::Position3D )
		.Append( VertexLayout::Normal )
	) );

	for( unsigned int i = 0; i < mesh.mNumVertices; i++ )
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
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

	std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

	bindablePtrs.push_back( std::make_unique<Bind::VertexBuffer>( gfx,vbuf ) );

	bindablePtrs.push_back( std::make_unique<Bind::IndexBuffer>( gfx,indices ) );

	auto pvs = std::make_unique<Bind::VertexShader>( gfx,L"PhongVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back( std::move( pvs ) );

	bindablePtrs.push_back( std::make_unique<Bind::PixelShader>( gfx,L"PhongPS.cso" ) );

	bindablePtrs.push_back( std::make_unique<Bind::InputLayout>( gfx,vbuf.GetLayout().GetD3DLayout(),pvsbc ) );

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	bindablePtrs.push_back( std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>( gfx,pmc,1u ) );

	return std::make_unique<Mesh>( gfx,std::move( bindablePtrs ) );
}
std::unique_ptr<Node> Model::ParseNode( const aiNode& node ) noexcept
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

	auto pNode = std::make_unique<Node>( node.mName.C_Str(),std::move( curMeshPtrs ),transform );
	for( size_t i = 0; i < node.mNumChildren; i++ )
	{
		pNode->AddChild( ParseNode( *node.mChildren[i] ) );
	}

	return pNode;
}
