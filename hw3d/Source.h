#pragma once
#include <string>
#include <memory>
#include "RenderGraphCompileException.h"

namespace Bind
{
	class Bindable;
	class BufferResource;
}

namespace Rgph
{
	class Source
	{
	public:
		const std::string& GetName() const noexcept;
		virtual void PostLinkValidate() const = 0;
		virtual std::shared_ptr<Bind::Bindable> YieldBindable();
		virtual std::shared_ptr<Bind::BufferResource> YieldBuffer();
		virtual ~Source() = default;
	protected:
		Source( std::string name );
	private:
		std::string name;
	};

	template<class T>
	class DirectBufferSource : public Source
	{
	public:
		static std::unique_ptr<DirectBufferSource> Make( std::string name,std::shared_ptr<T>& buffer )
		{
			return std::make_unique<DirectBufferSource>( std::move( name ),buffer );
		}
		DirectBufferSource( std::string name,std::shared_ptr<T>& buffer )
			:
			Source( std::move( name ) ),
			buffer( buffer )
		{}
		void PostLinkValidate() const
		{}
		std::shared_ptr<Bind::BufferResource> YieldBuffer() override
		{
			if( linked )
			{
				throw RGC_EXCEPTION( "Mutable output bound twice: " + GetName() );
			}
			linked = true;
			return buffer;
		}
	private:
		std::shared_ptr<T>& buffer;
		bool linked = false;
	};

	template<class T>
	class DirectBindableSource : public Source
	{
	public:
		static std::unique_ptr<DirectBindableSource> Make( std::string name,std::shared_ptr<T>& buffer )
		{
			return std::make_unique<DirectBindableSource>( std::move( name ),buffer );
		}
		DirectBindableSource( std::string name,std::shared_ptr<T>& bind )
			:
			Source( std::move( name ) ),
			bind( bind )
		{}
		void PostLinkValidate() const
		{}
		std::shared_ptr<Bind::Bindable> YieldBindable() override
		{
			return bind;
		}
	private:
		std::shared_ptr<T>& bind;
	};
}