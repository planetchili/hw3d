#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"

class Drawable;

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( Graphics& gfx ) noexcept = 0;
		virtual void InitializeParentReference( const Drawable& ) noexcept
		{}
		virtual std::string GetUID() const noexcept
		{
			assert( false );
			return "";
		}
		virtual ~Bindable() = default;
	protected:
		static ID3D11DeviceContext* GetContext( Graphics& gfx ) noexcept;
		static ID3D11Device* GetDevice( Graphics& gfx ) noexcept;
		static DxgiInfoManager& GetInfoManager( Graphics& gfx );
	};
}