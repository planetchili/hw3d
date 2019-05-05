#pragma once
#include "DrawableBase.h"
#include "ChiliMath.h"

template<class T>
class TestObject : public DrawableBase<T>
{
public:
	TestObject( Graphics& gfx,std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist )
		:
		r( rdist( rng ) ),
		droll( ddist( rng ) ),
		dpitch( ddist( rng ) ),
		dyaw( ddist( rng ) ),
		dphi( odist( rng ) ),
		dtheta( odist( rng ) ),
		dchi( odist( rng ) ),
		chi( adist( rng ) ),
		theta( adist( rng ) ),
		phi( adist( rng ) )
	{}
	void Update( float dt ) noexcept
	{
		roll = wrap_angle( roll + droll * dt );
		pitch = wrap_angle( pitch + dpitch * dt );
		yaw = wrap_angle( yaw + dyaw * dt );
		theta = wrap_angle( theta + dtheta * dt );
		phi = wrap_angle( phi + dphi * dt );
		chi = wrap_angle( chi + dchi * dt );
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		namespace dx = DirectX;
		return dx::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
			dx::XMMatrixTranslation( r,0.0f,0.0f ) *
			dx::XMMatrixRotationRollPitchYaw( theta,phi,chi );
	}
protected:
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
};
