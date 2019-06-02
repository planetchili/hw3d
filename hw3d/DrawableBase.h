#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"
#include "ConditionalNoexcept.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty();
	}
	static void AddStaticBind( std::unique_ptr<Bind::Bindable> bind ) noxnd
	{
		assert( "*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(Bind::IndexBuffer) );
		staticBinds.push_back( std::move( bind ) );
	}
	void AddStaticIndexBuffer( std::unique_ptr<Bind::IndexBuffer> ibuf ) noxnd
	{
		assert( "Attempting to add index buffer a second time" && pIndexBuffer == nullptr );
		pIndexBuffer = ibuf.get();
		staticBinds.push_back( std::move( ibuf ) );
	}
	void SetIndexFromStatic() noxnd
	{
		assert( "Attempting to add index buffer a second time" && pIndexBuffer == nullptr );
		for( const auto& b : staticBinds )
		{
			if( const auto p = dynamic_cast<Bind::IndexBuffer*>(b.get()) )
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert( "Failed to find index buffer in static binds" && pIndexBuffer != nullptr );
	}
private:
	const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bind::Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bind::Bindable>> DrawableBase<T>::staticBinds;