#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"
#include <memory>
#include <string>

class Drawable;
class TechniqueProbe;

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( Graphics& gfx ) noexcept = 0;
		virtual void InitializeParentReference( const Drawable& ) noexcept
		{}
		virtual void Accept( TechniqueProbe& )
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

	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}