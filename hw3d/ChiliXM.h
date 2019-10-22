#pragma once
#include <DirectXMath.h>

DirectX::XMFLOAT3 ExtractEulerAngles( const DirectX::XMFLOAT4X4& matrix );

DirectX::XMFLOAT3 ExtractTranslation( const DirectX::XMFLOAT4X4& matrix );

DirectX::XMMATRIX ScaleTranslation( DirectX::FXMMATRIX matrix,float scale );