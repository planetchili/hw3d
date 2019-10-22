#include "DynamicConstant.h"
#include "LayoutCodex.h"
#include "Vertex.h"
#include <cstring>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Material.h"
#include "Mesh.h"
#include "Testing.h"
#include "ChiliXM.h"

namespace dx = DirectX;

void TestDynamicMeshLoading()
{
	using namespace Dvtx;

	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( "Models\\brick_wall\\brick_wall.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	auto layout = VertexLayout{}
		.Append( VertexLayout::Position3D )
		.Append( VertexLayout::Normal )
		.Append( VertexLayout::Tangent )
		.Append( VertexLayout::Bitangent )
		.Append( VertexLayout::Texture2D );
	VertexBuffer buf{ std::move( layout ),*pScene->mMeshes[0] };

	for( auto i = 0ull,end = buf.Size(); i < end; i++ )
	{
		const auto a = buf[i].Attr<VertexLayout::Position3D>();
		const auto b = buf[i].Attr<VertexLayout::Normal>();
		const auto c = buf[i].Attr<VertexLayout::Tangent>();
		const auto d = buf[i].Attr<VertexLayout::Bitangent>();
		const auto e = buf[i].Attr<VertexLayout::Texture2D>();
	}
}

void TestMaterialSystemLoading( Graphics& gfx )
{
	std::string path = "Models\\brick_wall\\brick_wall.obj";
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( path,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	Material mat{ gfx,*pScene->mMaterials[1],path };
	Mesh mesh{ gfx,mat,*pScene->mMeshes[0] };
}

void TestScaleMatrixTranslation()
{
	auto tlMat = DirectX::XMMatrixTranslation( 20.f,30.f,40.f );
	tlMat = ScaleTranslation( tlMat,0.1f );
	dx::XMFLOAT4X4 f4;
	dx::XMStoreFloat4x4( &f4,tlMat );
	auto etl = ExtractTranslation( f4 );
	assert( etl.x == 2.f && etl.y == 3.f && etl.z == 4.f );
}

void TestDynamicConstant()
{
	using namespace std::string_literals;
	// data roundtrip tests
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Struct>( "butts"s );
		s["butts"s].Add<Dcb::Float3>( "pubes"s );
		s["butts"s].Add<Dcb::Float>( "dank"s );
		s.Add<Dcb::Float>( "woot"s );
		s.Add<Dcb::Array>( "arr"s );
		s["arr"s].Set<Dcb::Struct>( 4 );
		s["arr"s].T().Add<Dcb::Float3>( "twerk"s );
		s["arr"s].T().Add<Dcb::Array>( "werk"s );
		s["arr"s].T()["werk"s].Set<Dcb::Float>( 6 );
		s["arr"s].T().Add<Dcb::Array>( "meta"s );
		s["arr"s].T()["meta"s].Set<Dcb::Array>( 6 );
		s["arr"s].T()["meta"s].T().Set<Dcb::Matrix>( 4 );
		s["arr"s].T().Add<Dcb::Bool>( "booler" );

		// fails: duplicate symbol name
		// s.Add<Dcb::Bool>( "arr"s );

		// fails: bad symbol name
		//s.Add<Dcb::Bool>( "69man" );

		auto b = Dcb::Buffer( std::move( s ) );

		// fails to compile: conversion not in type map
		//b["woot"s] = "#"s;

		const auto sig = b.GetRootLayoutElement().GetSignature();


		{
			auto exp = 42.0f;
			b["woot"s] = exp;
			float act = b["woot"s];
			assert( act == exp );
		}
		{
			auto exp = 420.0f;
			b["butts"s]["dank"s] = exp;
			float act = b["butts"s]["dank"s];
			assert( act == exp );
		}
		{
			auto exp = 111.0f;
			b["arr"s][2]["werk"s][5] = exp;
			float act = b["arr"s][2]["werk"s][5];
			assert( act == exp );
		}
		{
			auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
			b["butts"s]["pubes"s] = exp;
			dx::XMFLOAT3 act = b["butts"s]["pubes"s];
			assert( !std::memcmp( &exp,&act,sizeof( DirectX::XMFLOAT3 ) ) );
		}
		{
			DirectX::XMFLOAT4X4 exp;
			dx::XMStoreFloat4x4(
				&exp,
				dx::XMMatrixIdentity()
			);
			b["arr"s][2]["meta"s][5][3] = exp;
			dx::XMFLOAT4X4 act = b["arr"s][2]["meta"s][5][3];
			assert( !std::memcmp( &exp,&act,sizeof( DirectX::XMFLOAT4X4 ) ) );
		}
		{
			auto exp = true;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert( act == exp );
		}
		{
			auto exp = false;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert( act == exp );
		}
		// exists
		{
			assert( b["butts"s]["pubes"s].Exists() );
			assert( !b["butts"s]["fubar"s].Exists() );
			if( auto ref = b["butts"s]["pubes"s]; ref.Exists() )
			{
				dx::XMFLOAT3 f = ref;
				assert( f.x == 69.0f );
			}
		}
		// set if exists
		{
			assert( b["butts"s]["pubes"s].SetIfExists( dx::XMFLOAT3{ 1.0f,2.0f,3.0f } ) );
			auto& f3 = static_cast<const dx::XMFLOAT3&>(b["butts"s]["pubes"s]);
			assert( f3.x == 1.0f && f3.y == 2.0f && f3.z == 3.0f );
			assert( !b["butts"s]["phubar"s].SetIfExists( dx::XMFLOAT3{ 2.0f,2.0f,7.0f } ) );
		}

		const auto& cb = b;
		{
			dx::XMFLOAT4X4 act = cb["arr"s][2]["meta"s][5][3];
			assert( act._11 == 1.0f );
		}
		// this doesn't compile: buffer is const
		// cb["arr"][2]["booler"] = true;
		// static_cast<bool&>(cb["arr"][2]["booler"]) = true;

		// this fails assertion: array out of bounds
		// cb["arr"s][200];

	}
	// size test array of arrays
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Array>( 6 );
		s["arr"].T().Set<Dcb::Matrix>( 4 );
		auto b = Dcb::Buffer( std::move( s ) );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 4u * 4u * 6u );
	}
	// size test array of structs with padding
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Struct>( 6 );
		s["arr"s].T().Add<Dcb::Float2>( "a" );
		s["arr"].T().Add<Dcb::Float3>( "b"s );
		auto b = Dcb::Buffer( std::move( s ) );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 2u * 6u );
	}
	// size test array of primitive that needs padding
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Float3>( 6 );
		auto b = Dcb::Buffer( std::move( s ) );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 6u );
	}
	// testing CookedLayout
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Float3>( 6 );
		auto cooked = Dcb::LayoutCodex::Resolve( std::move( s ) );
		// raw is cleared after donating
		s.Add<Dcb::Float>( "arr" );
		// fails to compile, cooked returns const&
		// cooked["arr"].Add<Dcb::Float>("buttman");
		auto b1 = Dcb::Buffer( cooked );
		b1["arr"][0] = dx::XMFLOAT3{ 69.0f,0.0f,0.0f };
		auto b2 = Dcb::Buffer( cooked );
		b2["arr"][0] = dx::XMFLOAT3{ 420.0f,0.0f,0.0f };
		assert( static_cast<dx::XMFLOAT3>(b1["arr"][0]).x == 69.0f );
		assert( static_cast<dx::XMFLOAT3>(b2["arr"][0]).x == 420.0f );
	}
	// specific testing scenario (packing error)
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add<Dcb::Float3>( "materialColor" );
		pscLayout.Add<Dcb::Float3>( "specularColor" );
		pscLayout.Add<Dcb::Float>( "specularWeight" );
		pscLayout.Add<Dcb::Float>( "specularGloss" );
		auto cooked = Dcb::LayoutCodex::Resolve( std::move( pscLayout ) );
		assert( cooked.GetSizeInBytes() == 48u );
	}
	// array non-packing
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add<Dcb::Array>( "arr" );
		pscLayout["arr"].Set<Dcb::Float>( 10 );
		auto cooked = Dcb::LayoutCodex::Resolve( std::move( pscLayout ) );
		assert( cooked.GetSizeInBytes() == 160u );
	}
	// array of struct w/ padding
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add<Dcb::Array>( "arr" );
		pscLayout["arr"].Set<Dcb::Struct>( 10 );
		pscLayout["arr"].T().Add<Dcb::Float3>( "x" );
		pscLayout["arr"].T().Add<Dcb::Float2>( "y" );
		auto cooked = Dcb::LayoutCodex::Resolve( std::move( pscLayout ) );
		assert( cooked.GetSizeInBytes() == 320u );
	}
	// testing pointer stuff
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Struct>( "butts"s );
		s["butts"s].Add<Dcb::Float3>( "pubes"s );
		s["butts"s].Add<Dcb::Float>( "dank"s );
		
		auto b = Dcb::Buffer( std::move( s ) );
		const auto exp = 696969.6969f;
		b["butts"s]["dank"s] = 696969.6969f;
		assert( (float&)b["butts"s]["dank"s] == exp );
		assert( *(float*)&b["butts"s]["dank"s] == exp );
		const auto exp2 = 42.424242f;
		*(float*)&b["butts"s]["dank"s] = exp2;
		assert( (float&)b["butts"s]["dank"s] == exp2 );
	}
	// specific testing scenario (packing error)
	{
		Dcb::RawLayout lay;
		lay.Add<Dcb::Bool>( "normalMapEnabled" );
		lay.Add<Dcb::Bool>( "specularMapEnabled" );
		lay.Add<Dcb::Bool>( "hasGlossMap" );
		lay.Add<Dcb::Float>( "specularPower" );
		lay.Add<Dcb::Float3>( "specularColor" );
		lay.Add<Dcb::Float>( "specularMapWeight" );

		auto buf = Dcb::Buffer( std::move( lay ) );
		assert( buf.GetSizeInBytes() == 32u );
	}
}