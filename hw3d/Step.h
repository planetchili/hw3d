#pragma once
#include <vector>
#include <memory>
#include "Bindable.h"
#include "Graphics.h"

class Step
{
public:
	Step( size_t targetPass_in )
		:
		targetPass{ targetPass_in }
	{}
	//template<class B>
	//B* QueryBindable() noexcept
	//{
	//	for( auto& pb : binds )
	//	{
	//		if( auto pt = dynamic_cast<T*>(pb.get()) )
	//		{
	//			return pt;
	//		}
	//	}
	//	return nullptr;
	//}
	void AddBindable( std::shared_ptr<Bind::Bindable> bind_in ) noexcept
	{
		bindables.push_back( std::move( bind_in ) );
	}
	void Submit( class FrameCommander& frame,const class Drawable& drawable ) const;
	void Bind( Graphics& gfx ) const
	{
		for( const auto& b : bindables )
		{
			b->Bind( gfx );
		}
	}
	void InitializeParentReferences( const class Drawable& parent ) noexcept;
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};