#pragma once
#include "ConditionalNoexcept.h"
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>

#define RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noxnd \
{ \
	assert( false && "Cannot resolve to" #eltype ); return 0u; \
}

#define LEAF_ELEMENT(eltype, systype) \
class eltype : public LayoutElement \
{ \
public: \
	using SystemType = systype; \
	size_t Resolve ## eltype() const noxnd override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + sizeof( systype ); \
	} \
protected: \
	size_t Finalize( size_t offset_in ) override \
	{ \
		offset = offset_in; \
		return offset_in + sizeof( systype ); \
	} \
};

#define REF_CONVERSION(eltype) \
operator eltype::SystemType&() noxnd \
{ \
	return *reinterpret_cast<eltype::SystemType*>(pBytes + offset + pLayout->Resolve ## eltype()); \
} \
eltype::SystemType& operator=( const eltype::SystemType& rhs ) noxnd \
{ \
	return static_cast<eltype::SystemType&>(*this) = rhs; \
}

#define PTR_CONVERSION(eltype) \
operator eltype::SystemType*() noxnd \
{ \
	return &static_cast<eltype::SystemType&>(ref); \
}


namespace Dcb
{
	class Struct;
	class Array;
	class Layout;
	namespace dx = DirectX;

	class LayoutElement
	{
		friend class Layout;
		friend class Array;
		friend class Struct;
	public:
		virtual ~LayoutElement()
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
		virtual LayoutElement& T()
		{
			assert( false );
			return *this;
		}
		virtual const LayoutElement& T() const
		{
			assert( false );
			return *this;
		}

		size_t GetOffsetBegin() const noexcept
		{
			return offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;
		size_t GetSizeInBytes() const noexcept
		{
			return GetOffsetEnd() - GetOffsetBegin();
		}

		template<typename T>
		Struct& Add( const std::string& key ) noxnd;
		template<typename T>
		Array& Set( size_t size ) noxnd;

		RESOLVE_BASE(Matrix)
		RESOLVE_BASE(Float4)
		RESOLVE_BASE(Float3)
		RESOLVE_BASE(Float2)
		RESOLVE_BASE(Float)
		RESOLVE_BASE(Bool)
	protected:
		virtual size_t Finalize( size_t offset ) = 0;
	protected:
		size_t offset = 0u;
	};


	LEAF_ELEMENT( Matrix,dx::XMFLOAT4X4 )
	LEAF_ELEMENT( Float4,dx::XMFLOAT4 )
	LEAF_ELEMENT( Float3,dx::XMFLOAT3 )
	LEAF_ELEMENT( Float2,dx::XMFLOAT2 )
	LEAF_ELEMENT( Float,float )
	LEAF_ELEMENT( Bool,BOOL )


	class Struct : public LayoutElement
	{
	public:
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
		Struct& Add( const std::string& name ) noxnd
		{
			elements.push_back( std::make_unique<T>() );
			if( !map.emplace( name,elements.back().get() ).second )
			{
				assert( false );
			}
			return *this;
		}
	protected:
		size_t Finalize( size_t offset_in ) override
		{
			assert( elements.size() != 0u );
			offset = offset_in;
			auto offsetNext = offset;
			for( auto& el : elements )
			{
				offsetNext = (*el).Finalize( offsetNext );
			}
			return GetOffsetEnd();
		}
	private:
		std::unordered_map<std::string,LayoutElement*> map;
		std::vector<std::unique_ptr<LayoutElement>> elements;
	};

	class Array : public LayoutElement
	{
	public:
		size_t GetOffsetEnd() const noexcept override final
		{
			assert( pElement );
			return GetOffsetBegin() + pElement->GetSizeInBytes() * size;
		}
		template<typename T>
		Array& Set( size_t size_in ) noxnd
		{
			pElement = std::make_unique<T>();
			size = size_in;
			return *this;
		}
		LayoutElement& T() override final
		{
			return *pElement;
		}
		const LayoutElement& T() const override final
		{
			return *pElement;
		}
	protected:
		size_t Finalize( size_t offset_in ) override
		{
			assert( size != 0u && pElement );
			offset = offset_in;
			pElement->Finalize( offset_in );
			return offset + pElement->GetSizeInBytes() * size;
		}
	private:
		size_t size = 0u;
		std::unique_ptr<LayoutElement> pElement;
	};



	class Layout
	{
	public:
		Layout()
			:
			pLayout( std::make_shared<Struct>() )
		{}
		LayoutElement& operator[]( const char* key )
		{
			assert( !finalized && "cannot modify finalized layout" );
			return (*pLayout)[key];
		}
		size_t GetSizeInBytes() const noexcept
		{
			return pLayout->GetSizeInBytes();
		}
		template<typename T>
		LayoutElement& Add( const std::string& key ) noxnd
		{
			assert( !finalized && "cannot modify finalized layout" );
			return pLayout->Add<T>( key );
		}
		std::shared_ptr<LayoutElement> Finalize()
		{
			pLayout->Finalize( 0 );
			finalized = true;
			return pLayout;
		}
	private:
		bool finalized = false;
		std::shared_ptr<LayoutElement> pLayout;
	};

	class ElementRef
	{
	public:
		class Ptr
		{
		public:
			Ptr( ElementRef& ref )
				:
				ref( ref )
			{}

			PTR_CONVERSION( Matrix )
			PTR_CONVERSION( Float4 )
			PTR_CONVERSION( Float3 )
			PTR_CONVERSION( Float2 )
			PTR_CONVERSION( Float )
			PTR_CONVERSION( Bool )
		private:
			ElementRef& ref;
		};
	public:
		ElementRef( const LayoutElement* pLayout,char* pBytes,size_t offset )
			:
			offset( offset ),
			pLayout( pLayout ),
			pBytes( pBytes )
		{}
		ElementRef operator[]( const char* key ) noxnd
		{
			return { &(*pLayout)[key],pBytes,offset };
		}
		ElementRef operator[]( size_t index ) noxnd
		{
			const auto& t = pLayout->T();
			return { &t,pBytes,offset + t.GetSizeInBytes() * index };
		}
		Ptr operator&() noxnd
		{
			return { *this };
		}

		REF_CONVERSION(Matrix)
		REF_CONVERSION(Float4)
		REF_CONVERSION(Float3)
		REF_CONVERSION(Float2)
		REF_CONVERSION(Float)
		REF_CONVERSION(Bool)
	private:
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer( Layout& lay )
			:
			pLayout( std::static_pointer_cast<Struct>( lay.Finalize() ) ),
			bytes( pLayout->GetOffsetEnd() )
		{}
		ElementRef operator[]( const char* key ) noxnd
		{
			return { &(*pLayout)[key],bytes.data(),0u };
		}
		const char* GetData() const noexcept
		{
			return bytes.data();
		}
		size_t GetSizeInBytes() const noexcept
		{
			return bytes.size();
		}
		const LayoutElement& GetLayout() const noexcept
		{
			return *pLayout;
		}
		std::shared_ptr<LayoutElement> CloneLayout() const
		{
			return pLayout;
		}
	private:
		std::shared_ptr<Struct> pLayout;
		std::vector<char> bytes;
	};
	

	// must come after Definitions of Struct and Array
	template<typename T>
	Struct& LayoutElement::Add( const std::string& key ) noxnd
	{
		auto ps = dynamic_cast<Struct*>(this);
		assert( ps != nullptr );
		return ps->Add<T>( key );
	}

	template<typename T>
	Array& LayoutElement::Set( size_t size ) noxnd
	{
		auto pa = dynamic_cast<Array*>(this);
		assert( pa != nullptr );
		return pa->Set<T>( size );
	}
}