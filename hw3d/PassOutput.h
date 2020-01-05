#pragma once
#include <string>
#include <memory>
#include "RenderGraphCompileException.h"

namespace Bind
{
	class Bindable;
	class BufferResource;
}

class PassOutput
{
public:
	const std::string& GetName() const noexcept;
	virtual void PostLinkValidate() const = 0;
	virtual std::shared_ptr<Bind::Bindable> YieldImmutable();
	virtual std::shared_ptr<Bind::BufferResource> YieldBuffer();
	virtual ~PassOutput() = default;
protected:
	PassOutput( std::string name );
private:
	std::string name;
};

template<class T>
class BufferOutput : public PassOutput
{
public:
	static std::unique_ptr<BufferOutput> Make( std::string name,std::shared_ptr<T>& buffer )
	{
		return std::make_unique<BufferOutput>( std::move( name ),buffer );
	}
	BufferOutput( std::string name,std::shared_ptr<T>& buffer )
		:
		PassOutput( std::move( name ) ),
		buffer( buffer )
	{}
	void PostLinkValidate() const
	{}
	std::shared_ptr<Bind::BufferResource> YieldBuffer()
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
class ImmutableOutput : public PassOutput
{
public:
	static std::unique_ptr<ImmutableOutput> Make( std::string name,std::shared_ptr<T>& buffer )
	{
		return std::make_unique<ImmutableOutput>( std::move( name ),buffer );
	}
	ImmutableOutput( std::string name,std::shared_ptr<T>& bind )
		:
		PassOutput( std::move( name ) ),
		bind( bind )
	{}
	void PostLinkValidate() const
	{}
	std::shared_ptr<Bind::Bindable> YieldImmutable()
	{
		return bind;
	}
private:
	std::shared_ptr<T>& bind;
};