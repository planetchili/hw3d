#pragma once
#include <DirectXMath.h>
#include <string>
#include "Projection.h"

class Graphics;

class Camera
{
public:
	Camera( std::string name,DirectX::XMFLOAT3 homePos = {0.0f,0.0f,0.0f},float homePitch = 0.0f,float homeYaw = 0.0f ) noexcept;
	void BindToGraphics( Graphics& gfx ) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWidgets() noexcept;
	void Reset() noexcept;
	void Rotate( float dx,float dy ) noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	const std::string& GetName() const noexcept;
private:
	std::string name;
	DirectX::XMFLOAT3 homePos;
	float homePitch;
	float homeYaw;
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	Projection proj;
};