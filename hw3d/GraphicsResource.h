#pragma once
#include "Graphics.h"

class Graphics;

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext( Graphics& gfx ) noexcept;
	static ID3D11Device* GetDevice( Graphics& gfx ) noexcept;
	static DxgiInfoManager& GetInfoManager( Graphics& gfx );
};