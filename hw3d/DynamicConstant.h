#pragma once
#include "ConditionalNoexcept.h"
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>

namespace Dcb
{
	namespace dx = DirectX;
	class LayoutElement
	{
	public:
		LayoutElement( size_t offset )
			:
			offset( offset )
		{}
		virtual LayoutElement& operator[]( const char* )
		{
			assert( false && "cannot access member on non Struct" );
			return *this;
		}
		virtual const LayoutElement& operator[]( const char* key ) const
		{
			assert( false && "cannot access member on non Struct" );
			return *this;
		}
		size_t GetOffsetBegin() const noexcept
		{
			return offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;

		virtual size_t ResolveFloat3() const noxnd
		{
			assert( false && "Cannot resolve LayoutElement type" );
			return 0;
		}
	private:
		size_t offset;
	};

	class Float3 : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		size_t ResolveFloat3() const noxnd
		{
			return GetOffsetBegin();
		}
		size_t GetOffsetEnd() const noexcept override
		{
			return GetOffsetBegin() + sizeof( dx::XMFLOAT3 );
		}
	};

	class Struct : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		LayoutElement& operator[]( const char* key ) override final
		{
			return *map.at( key );
		}
		const LayoutElement& operator[]( const char* key ) const override final
		{
			return *map.at( key );
		}
		size_t GetOffsetEnd() const noexcept override final
		{
			return elements.empty() ? GetOffsetBegin() : elements.back()->GetOffsetEnd();
		}
		template<typename T>
		void Add( const std::string& name )
		{
			elements.push_back( std::make_unique<T>( GetOffsetEnd() ) );
			if( !map.emplace( name,elements.back().get() ).second )
			{
				assert( false );
			}
		}
	private:
		std::unordered_map<std::string,LayoutElement*> map;
		std::vector<std::unique_ptr<LayoutElement>> elements;
	};

	class ElementRef
	{
	public:
		ElementRef( const LayoutElement* pLayout,char* pBytes )
			:
			pLayout( pLayout ),
			pBytes( pBytes )
		{}
		ElementRef operator[]( const char* key ) noxnd
		{
			return { &(*pLayout)[key],pBytes };
		}

		operator dx::XMFLOAT3&() noxnd
		{
			return *reinterpret_cast<dx::XMFLOAT3*>(pBytes + pLayout->ResolveFloat3());
		}
		dx::XMFLOAT3& operator=( const dx::XMFLOAT3& rhs ) noxnd
		{
			auto& ref = *reinterpret_cast<dx::XMFLOAT3*>(pBytes + pLayout->ResolveFloat3());
			ref = rhs;
			return ref;
		}
	private:
		const class LayoutElement* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer( const Struct& pLayout )
			:
			pLayout( &pLayout ),
			bytes( pLayout.GetOffsetEnd() )
		{}
		ElementRef operator[]( const char* key ) noxnd
		{
			return { &(*pLayout)[key],bytes.data() };
		}
	private:
		const class Struct* pLayout;
		std::vector<char> bytes;
	};
}