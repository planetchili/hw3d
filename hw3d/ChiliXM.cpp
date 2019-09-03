#include "ChiliXM.h"

DirectX::XMFLOAT3 ExtractEulerAngles( const DirectX::XMFLOAT4X4& mat )
{
	DirectX::XMFLOAT3 euler;
	
	euler.x = asinf( -mat._32 );                  // Pitch
	if( cosf( euler.x ) > 0.0001 )                // Not at poles
	{
		euler.y = atan2f( mat._31,mat._33 );      // Yaw
		euler.z = atan2f( mat._12,mat._22 );      // Roll
	}
	else
	{
		euler.y = 0.0f;                           // Yaw
		euler.z = atan2f( -mat._21,mat._11 );     // Roll
	}

	return euler;
}

DirectX::XMFLOAT3 ExtractTranslation( const DirectX::XMFLOAT4X4 & matrix )
{
	return { matrix._41,matrix._42,matrix._43 };
}
