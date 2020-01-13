#pragma once
#include <string>
#include <memory>
#include <typeinfo>
#include "PassOutput.h"
#include "RenderGraphCompileException.h"
#include "ChiliUtil.h"
#include "Bindable.h"
#include "BufferResource.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include <sstream>
#include <cctype>
#include <type_traits>

namespace Bind
{
	class Bindable;
}

class Pass;

class PassInput
{
public:
	const std::string& GetRegisteredName() const noexcept;
	const std::string& GetPassName() const noexcept;
	const std::string& GetOutputName() const noexcept;
	void SetTarget( std::string passName,std::string outputName );
	virtual void Bind( PassOutput& out ) = 0;
	virtual void PostLinkValidate() const = 0;
	virtual ~PassInput() = default;
protected:
	PassInput( std::string registeredName );
private:
	std::string registeredName;
	std::string passName;
	std::string outputName;
};

template<class T>
class BufferInput : public PassInput
{
	static_assert( std::is_base_of_v<Bind::BufferResource,T>,"BufferInput target type must be a BufferResource type" );
public:
	static std::unique_ptr<PassInput> Make( std::string registeredName,std::shared_ptr<T>& target )
	{
		return std::make_unique<BufferInput>( std::move( registeredName ),target );
	}
	void PostLinkValidate() const override
	{
		if( !linked )
		{
			throw RGC_EXCEPTION( "Unlinked input: " + GetRegisteredName() );
		}
	}
	void Bind( PassOutput& out ) override
	{
		auto p = std::dynamic_pointer_cast<T>(out.YieldBuffer());
		if( !p )
		{
			std::ostringstream oss;
			oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
				<< " { " << typeid(T).name() << " } not compatible with { " << typeid(*out.YieldBuffer().get()).name() << " }";
			throw RGC_EXCEPTION( oss.str() );
		}
		target = std::move( p );
		linked = true;
	}
	BufferInput( std::string registeredName,std::shared_ptr<T>& bind )
		:
		PassInput( std::move( registeredName ) ),
		target( bind )
	{}
private:
	std::shared_ptr<T>& target;
	bool linked = false;
};

template<class T>
class ImmutableInput : public PassInput
{
	static_assert( std::is_base_of_v<Bind::Bindable,T>,"ImmutableInput target type must be a Bindable type" );
public:
	static std::unique_ptr<PassInput> Make( std::string registeredName,std::shared_ptr<T>& target )
	{
		return std::make_unique<ImmutableInput>( std::move( registeredName ),target );
	}
	void PostLinkValidate() const override
	{
		if( !linked )
		{
			throw RGC_EXCEPTION( "Unlinked input: " + GetRegisteredName() );
		}
	}
	void Bind( PassOutput& out ) override
	{
		auto p = std::dynamic_pointer_cast<T>(out.YieldImmutable());
		if( !p )
		{
			std::ostringstream oss;
			oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
				<< " { " << typeid(T).name() << " } does not match { " << typeid(*out.YieldImmutable().get()).name() << " }";
			throw RGC_EXCEPTION( oss.str() );
		}
		target = std::move( p );
		linked = true;
	}
	ImmutableInput( std::string registeredName,std::shared_ptr<T>& target )
		:
		PassInput( std::move( registeredName ) ),
		target( target )
	{}
private:
	std::shared_ptr<T>& target;
	bool linked = false;
};