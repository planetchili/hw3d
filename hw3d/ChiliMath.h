#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq( const T& x )
{
	return x * x;
}

template<typename T>
T wrap_angle( T theta )
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = fmod( theta,twoPi );
	if( mod > (T)PI_D )
	{
		return mod - twoPi;
	}
	else if( mod < (T)PI_D )
	{
		return mod + twoPi;
	}
	return mod;
}

template<typename T>
constexpr T interpolate( const T& src,const T& dst,float alpha )
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad( T deg )
{
	return deg * PI / (T)180.0;
}