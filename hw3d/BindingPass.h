#pragma once
#include "Pass.h"
#include "Sink.h"

namespace Bind
{
	class Bindable;
}

namespace Rgph
{
	class BindingPass : public Pass
	{
	protected:
		BindingPass( std::string name,std::vector<std::shared_ptr<Bind::Bindable>> binds = {} );
		void AddBind( std::shared_ptr<Bind::Bindable> bind ) noexcept;
		void BindAll( Graphics& gfx ) const noexcept;
		void Finalize() override;
	protected:
		template<class T>
		void AddBindSink( std::string name )
		{
			const auto index = binds.size();
			binds.emplace_back();
			RegisterSink( std::make_unique<ContainerBindableSink<T>>( std::move( name ),binds,index ) );
		}
		std::shared_ptr<Bind::RenderTarget> renderTarget;
		std::shared_ptr<Bind::DepthStencil> depthStencil;
	private:
		void BindBufferResources( Graphics& gfx ) const noxnd;
	private:
		std::vector<std::shared_ptr<Bind::Bindable>> binds;
	};
}