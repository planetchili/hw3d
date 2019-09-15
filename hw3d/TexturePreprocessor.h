#pragma once

#include "Surface.h"
#include <string>
#include <DirectXMath.h>


class TexturePreprocessor
{
public:
	static void FlipYAllNormalMapsInObj( const std::string& objPath );
private:
	template<typename F>
	static void TransformFile( const std::string& pathIn,const std::string& pathOut,F&& func );
	static DirectX::XMVECTOR ColorToVector( Surface::Color c ) noexcept;
	static Surface::Color VectorToColor( DirectX::FXMVECTOR n ) noexcept;
};
