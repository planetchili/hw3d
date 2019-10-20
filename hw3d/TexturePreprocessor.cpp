#include "TexturePreprocessor.h"
#include <filesystem>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ChiliMath.h"
#include "ModelException.h"


template<typename F>
inline void TexturePreprocessor::TransformSurface( Surface& surf,F&& func )
{
	const auto width = surf.GetWidth();
	const auto height = surf.GetHeight();
	for( unsigned int y = 0; y < height; y++ )
	{
		for( unsigned int x = 0; x < width; x++ )
		{
			const auto n = ColorToVector( surf.GetPixel( x,y ) );
			surf.PutPixel( x,y,VectorToColor( func( n,x,y ) ) );
		}
	}
}

template<typename F>
inline void TexturePreprocessor::TransformFile( const std::string& pathIn,const std::string& pathOut,F&& func )
{
	auto surf = Surface::FromFile( pathIn );
	TransformSurface( surf,func );
	surf.Save( pathOut );
}

void TexturePreprocessor::FlipYAllNormalMapsInObj( const std::string & objPath )
{
	const auto rootPath = std::filesystem::path{ objPath }.parent_path().string() + "\\";

	// load scene from .obj file to get our list of normal maps in the materials
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile( objPath.c_str(),0u );
	if( pScene == nullptr )
	{
		throw ModelException( __LINE__,__FILE__,imp.GetErrorString() );
	}

	// loop through materials and process any normal maps
	for( auto i = 0u; i < pScene->mNumMaterials; i++ )
	{
		const auto& mat = *pScene->mMaterials[i];
		aiString texFileName;
		if( mat.GetTexture( aiTextureType_NORMALS,0,&texFileName ) == aiReturn_SUCCESS )
		{
			const auto path = rootPath + texFileName.C_Str();
			FlipYNormalMap( path,path );
		}
	}
}

void TexturePreprocessor::FlipYNormalMap( const std::string& pathIn,const std::string& pathOut )
{
	// function for processing each normal in texture
	using namespace DirectX;
	const auto flipY = XMVectorSet( 1.0f,-1.0f,1.0f,1.0f );
	const auto ProcessNormal = [flipY]( FXMVECTOR n,int x,int y ) -> XMVECTOR
	{
		return XMVectorMultiply( n,flipY );
	};
	// execute processing over every texel in file
	TransformFile( pathIn,pathOut,ProcessNormal );
}

void TexturePreprocessor::ValidateNormalMap( const std::string& pathIn,float thresholdMin,float thresholdMax )
{
	OutputDebugStringA( ("Validating normal map [" + pathIn + "]\n").c_str() );
	// function for processing each normal in texture
	using namespace DirectX;
	auto sum = XMVectorZero();
	const auto ProcessNormal = [thresholdMin,thresholdMax,&sum]( FXMVECTOR n,int x,int y ) -> XMVECTOR
	{
		const float len = XMVectorGetX( XMVector3Length( n ) );
		const float z = XMVectorGetZ( n );
		if( len < thresholdMin || len > thresholdMax )
		{
			XMFLOAT3 vec;
			XMStoreFloat3( &vec,n );
			std::ostringstream oss;
			oss << "Bad normal length: " << len << " at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA( oss.str().c_str() );
		}
		if( z < 0.0f )
		{
			XMFLOAT3 vec;
			XMStoreFloat3( &vec,n );
			std::ostringstream oss;
			oss << "Bad normal Z direction at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA( oss.str().c_str() );
		}
		sum = XMVectorAdd( sum,n );
		return n;
	};
	// execute the validation for each texel
	auto surf = Surface::FromFile( pathIn );
	TransformSurface( surf,ProcessNormal );
	// output bias
	{
		XMFLOAT2 sumv;
		XMStoreFloat2( &sumv,sum );
		std::ostringstream oss;
		oss << "Normal map biases: (" << sumv.x << "," << sumv.y << ")\n";
		OutputDebugStringA( oss.str().c_str() );		
	}
}

void TexturePreprocessor::MakeStripes( const std::string& pathOut,int size,int stripeWidth )
{
	// make sure texture dimension is power of 2
	auto power = log2( size );
	assert( modf( power,&power ) == 0.0 );
	// make sure stripe width enables at least 2 stripes
	assert( stripeWidth < size / 2 );

	Surface s( size,size );
	for( int y = 0; y < size; y++ )
	{
		for( int x = 0; x < size; x++ )
		{
			Surface::Color c = { 0,0,0 };
			if( (x / stripeWidth) % 2 == 0 )
			{
				c = { 255,255,255 };
			}
			s.PutPixel( x,y,c );
		}
	}
	s.Save( pathOut );
}

DirectX::XMVECTOR TexturePreprocessor::ColorToVector( Surface::Color c ) noexcept
{
	using namespace DirectX;
	auto n = XMVectorSet( (float)c.GetR(),(float)c.GetG(),(float)c.GetB(),0.0f );
	const auto all255 = XMVectorReplicate( 2.0f / 255.0f );
	n = XMVectorMultiply( n,all255 );
	const auto all1 = XMVectorReplicate( 1.0f );
	n = XMVectorSubtract( n,all1 );
	return n;
}

Surface::Color TexturePreprocessor::VectorToColor( DirectX::FXMVECTOR n ) noexcept
{
	using namespace DirectX;
	const auto all1 = XMVectorReplicate( 1.0f );
	XMVECTOR nOut = XMVectorAdd( n,all1 );
	const auto all255 = XMVectorReplicate( 255.0f / 2.0f );
	nOut = XMVectorMultiply( nOut,all255 );
	XMFLOAT3 floats;
	XMStoreFloat3( &floats,nOut );
	return {
		(unsigned char)round( floats.x ),
		(unsigned char)round( floats.y ),
		(unsigned char)round( floats.z ),
	};
}
