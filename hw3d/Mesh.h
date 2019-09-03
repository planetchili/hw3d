#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ConditionalNoexcept.h"
#include "ConstantBuffers.h"
#include <type_traits>
#include <filesystem>
#include "imgui/imgui.h"


class ModelException : public ChiliException
{
public:
	ModelException( int line,const char* file,std::string note ) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

class Mesh : public Drawable
{
public:
	Mesh( Graphics& gfx,std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	struct PSMaterialConstantFullmonte
	{
		BOOL  normalMapEnabled = TRUE;
		BOOL  specularMapEnabled = TRUE;
		BOOL  hasGlossMap = FALSE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
		float specularMapWeight = 0.671f;
	}; 
	struct PSMaterialConstantNotex
	{
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f,0.65f,0.65f,1.0f };
		float specularPower = 120.0f;
		float padding[3];
	};
public:
	Node( int id,const std::string& name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform ) noxnd;
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
	void SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	void ShowTree( Node*& pSelectedNode ) const noexcept;
	template<class T>
	bool ControlMeDaddy( Graphics& gfx,T& c )
	{
		if( meshPtrs.empty() )
		{
			return false;
		}

		if constexpr( std::is_same<T,PSMaterialConstantFullmonte>::value )
		{
			if( auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>() )
			{
				ImGui::Text( "Material" );

				bool normalMapEnabled = (bool)c.normalMapEnabled;
				ImGui::Checkbox( "Norm Map",&normalMapEnabled );
				c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)c.specularMapEnabled;
				ImGui::Checkbox( "Spec Map",&specularMapEnabled );
				c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				bool hasGlossMap = (bool)c.hasGlossMap;
				ImGui::Checkbox( "Gloss Alpha",&hasGlossMap );
				c.hasGlossMap = hasGlossMap ? TRUE : FALSE;

				ImGui::SliderFloat( "Spec Weight",&c.specularMapWeight,0.0f,2.0f );

				ImGui::SliderFloat( "Spec Pow",&c.specularPower,0.0f,1000.0f,"%f",5.0f );

				ImGui::ColorPicker3( "Spec Color",reinterpret_cast<float*>(&c.specularColor) );

				pcb->Update( gfx,c );
				return true;
			}
		}
		else if constexpr( std::is_same<T,PSMaterialConstantNotex>::value )
		{
			if( auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>() )
			{
				ImGui::Text( "Material" );

				ImGui::ColorPicker3( "Spec Color",reinterpret_cast<float*>(&c.specularColor) );

				ImGui::SliderFloat( "Spec Pow",&c.specularPower,0.0f,1000.0f,"%f",5.0f );

				ImGui::ColorPicker3( "Diff Color",reinterpret_cast<float*>(&c.materialColor) );

				pcb->Update( gfx,c );
				return true;
			}
		}
		return false;
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

class Model
{
public:
	Model( Graphics& gfx,const std::string& pathString,float scale = 1.0f );
	void Draw( Graphics& gfx ) const noxnd;
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