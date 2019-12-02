#define DCB_IMPL_SOURCE
#include "DynamicConstant.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "LayoutCodex.h"


namespace Dcb
{
	struct ExtraData
	{
		struct Struct : public LayoutElement::ExtraDataBase
		{
			std::vector<std::pair<std::string,LayoutElement>> layoutElements;
		};
		struct Array : public LayoutElement::ExtraDataBase
		{
			std::optional<LayoutElement> layoutElement;
			size_t size;
		};
	};

	std::string LayoutElement::GetSignature() const noxnd
	{
		switch( type )
		{
		#define X(el) case el: return Map<el>::code;
		LEAF_ELEMENT_TYPES
		#undef X
		case Struct:
			return GetSignatureForStruct();
		case Array:
			return GetSignatureForArray();
		default:
			assert( "Bad type in signature generation" && false );
			return "???";
		}
	}
	bool LayoutElement::Exists() const noexcept
	{
		return type != Empty;
	}
	std::pair<size_t,const LayoutElement*> LayoutElement::CalculateIndexingOffset( size_t offset,size_t index ) const noxnd
	{
		assert( "Indexing into non-array" && type == Array );
		const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		assert( index < data.size );
		return { offset + data.layoutElement->GetSizeInBytes() * index,&*data.layoutElement };
	}
	LayoutElement& LayoutElement::operator[]( const std::string& key ) noxnd
	{
		assert( "Keying into non-struct" && type == Struct );
		for( auto& mem : static_cast<ExtraData::Struct&>(*pExtraData).layoutElements )
		{
			if( mem.first == key )
			{
				return mem.second;
			}
		}
		return GetEmptyElement();
	}
	const LayoutElement& LayoutElement::operator[]( const std::string& key ) const noxnd
	{
		return const_cast<LayoutElement&>(*this)[key];
	}
	LayoutElement& LayoutElement::T() noxnd
	{
		assert( "Accessing T of non-array" && type == Array );
		return *static_cast<ExtraData::Array&>(*pExtraData).layoutElement;
	}
	const LayoutElement& LayoutElement::T() const noxnd
	{
		return const_cast<LayoutElement&>(*this).T();
	}
	size_t LayoutElement::GetOffsetBegin() const noxnd
	{
		return *offset;
	}
	size_t LayoutElement::GetOffsetEnd() const noxnd
	{
		switch( type )
		{
		#define X(el) case el: return *offset + Map<el>::hlslSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case Struct:
		{
			const auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
			return AdvanceToBoundary( data.layoutElements.back().second.GetOffsetEnd() );
		}
		case Array:
		{
			const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
			return *offset + AdvanceToBoundary( data.layoutElement->GetSizeInBytes() ) * data.size;
		}
		default:
			assert( "Tried to get offset of empty or invalid element" && false );
			return 0u;
		}
	}
	size_t LayoutElement::GetSizeInBytes() const noxnd
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}
	LayoutElement& LayoutElement::Add( Type addedType,std::string name ) noxnd
	{
		assert( "Add to non-struct in layout" && type == Struct );
		assert( "invalid symbol name in Struct" && ValidateSymbolName( name ) );
		auto& structData = static_cast<ExtraData::Struct&>(*pExtraData);
		for( auto& mem : structData.layoutElements )
		{
			if( mem.first == name )
			{
				assert( "Adding duplicate name to struct" && false );
			}
		}
		structData.layoutElements.emplace_back( std::move( name ),LayoutElement{ addedType } );
		return *this;
	}
	LayoutElement& LayoutElement::Set( Type addedType,size_t size ) noxnd
	{
		assert( "Set on non-array in layout" && type == Array );
		assert( size != 0u );
		auto& arrayData = static_cast<ExtraData::Array&>(*pExtraData);
		arrayData.layoutElement = { addedType };
		arrayData.size = size;
		return *this;
	}
	LayoutElement::LayoutElement( Type typeIn ) noxnd
		:
		type{ typeIn }
	{
		assert( typeIn != Empty );
		if( typeIn == Struct )
		{
			pExtraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
		}
		else if( typeIn == Array )
		{
			pExtraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
		}
	}
	size_t LayoutElement::Finalize( size_t offsetIn ) noxnd
	{
		switch( type )
		{
		#define X(el) case el: offset = AdvanceIfCrossesBoundary( offsetIn,Map<el>::hlslSize ); return *offset + Map<el>::hlslSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case Struct:
			return FinalizeForStruct( offsetIn );
		case Array:
			return FinalizeForArray( offsetIn );
		default:
			assert( "Bad type in size computation" && false );
			return 0u;
		}
	}
	std::string LayoutElement::GetSignatureForStruct() const noxnd
	{
		using namespace std::string_literals;
		auto sig = "St{"s;
		for( const auto& el : static_cast<ExtraData::Struct&>(*pExtraData).layoutElements )
		{
			sig += el.first + ":"s + el.second.GetSignature() + ";"s;
		}
		sig += "}"s;
		return sig;
	}
	std::string LayoutElement::GetSignatureForArray() const noxnd
	{
		using namespace std::string_literals;
		const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		return "Ar:"s + std::to_string( data.size ) + "{"s + data.layoutElement->GetSignature() + "}"s;
	}
	size_t LayoutElement::FinalizeForStruct( size_t offsetIn )
	{
		auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
		assert( data.layoutElements.size() != 0u );
		offset = AdvanceToBoundary( offsetIn );
		auto offsetNext = *offset;
		for( auto& el : data.layoutElements )
		{
			offsetNext = el.second.Finalize( offsetNext );
		}
		return offsetNext;
	}
	size_t LayoutElement::FinalizeForArray( size_t offsetIn )
	{
		auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		assert( data.size != 0u );
		offset = AdvanceToBoundary( offsetIn );
		data.layoutElement->Finalize( *offset );
		return GetOffsetEnd();
	}
	bool LayoutElement::CrossesBoundary( size_t offset,size_t size ) noexcept
	{
		const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
	}
	size_t LayoutElement::AdvanceIfCrossesBoundary( size_t offset,size_t size ) noexcept
	{
		return CrossesBoundary( offset,size ) ? AdvanceToBoundary( offset ) : offset;
	}
	size_t LayoutElement::AdvanceToBoundary( size_t offset ) noexcept
	{
		return offset + (16u - offset % 16u) % 16u;
	}
	bool LayoutElement::ValidateSymbolName( const std::string& name ) noexcept
	{
		// symbols can contain alphanumeric and underscore, must not start with digit
		return !name.empty() && !std::isdigit( name.front() ) &&
			std::all_of( name.begin(),name.end(),[]( char c ) {
				return std::isalnum( c ) || c == '_';
			}
		);
	}




	Layout::Layout( std::shared_ptr<LayoutElement> pRoot ) noexcept
		:
		pRoot{ std::move( pRoot ) }
	{}
	size_t Layout::GetSizeInBytes() const noexcept
	{
		return pRoot->GetSizeInBytes();
	}
	std::string Layout::GetSignature() const noxnd
	{
		return pRoot->GetSignature();
	}


	RawLayout::RawLayout() noexcept
		:
		Layout{ std::shared_ptr<LayoutElement>{ new LayoutElement( Struct ) } }
	{}
	LayoutElement& RawLayout::operator[]( const std::string& key ) noxnd
	{
		return (*pRoot)[key];
	}
	std::shared_ptr<LayoutElement> RawLayout::DeliverRoot() noexcept
	{
		auto temp = std::move( pRoot );
		temp->Finalize( 0 );
		*this = RawLayout();
		return std::move( temp );
	}
	void RawLayout::ClearRoot() noexcept
	{
		*this = RawLayout();
	}


	CookedLayout::CookedLayout( std::shared_ptr<LayoutElement> pRoot ) noexcept
		:
		Layout( std::move( pRoot ) )
	{}
	std::shared_ptr<LayoutElement> CookedLayout::RelinquishRoot() const noexcept
	{
		return std::move( pRoot );
	}
	std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept
	{
		return pRoot;
	}
	const LayoutElement& CookedLayout::operator[]( const std::string& key ) const noxnd
	{
		return (*pRoot)[key];
	}





	bool ConstElementRef::Exists() const noexcept
	{
		return pLayout->Exists();
	}
	ConstElementRef ConstElementRef::operator[]( const std::string& key ) const noxnd
	{
		return { &(*pLayout)[key],pBytes,offset };
	}
	ConstElementRef ConstElementRef::operator[]( size_t index ) const noxnd
	{
		const auto indexingData = pLayout->CalculateIndexingOffset( offset,index );
		return { indexingData.second,pBytes,indexingData.first };
	}
	ConstElementRef::Ptr ConstElementRef::operator&() const noxnd
	{
		return Ptr{ this };
	}
	ConstElementRef::ConstElementRef( const LayoutElement* pLayout,const char* pBytes,size_t offset ) noexcept
		:
		offset( offset ),
		pLayout( pLayout ),
		pBytes( pBytes )
	{}
	ConstElementRef::Ptr::Ptr( const ConstElementRef* ref ) noexcept : ref( ref )
	{}
	

	ElementRef::operator ConstElementRef() const noexcept
	{
		return { pLayout,pBytes,offset };
	}
	bool ElementRef::Exists() const noexcept
	{
		return pLayout->Exists();
	}
	ElementRef ElementRef::operator[]( const std::string& key ) const noxnd
	{
		return { &(*pLayout)[key],pBytes,offset };
	}
	ElementRef ElementRef::operator[]( size_t index ) const noxnd
	{
		const auto indexingData = pLayout->CalculateIndexingOffset( offset,index );
		return { indexingData.second,pBytes,indexingData.first };
	}
	ElementRef::Ptr ElementRef::operator&() const noxnd
	{
		return Ptr{ const_cast<ElementRef*>(this) };
	}
	ElementRef::ElementRef( const LayoutElement* pLayout,char* pBytes,size_t offset ) noexcept
		:
		offset( offset ),
		pLayout( pLayout ),
		pBytes( pBytes )
	{}
	ElementRef::Ptr::Ptr( ElementRef* ref ) noexcept : ref( ref )
	{}




	Buffer::Buffer( RawLayout&& lay ) noxnd
		:
		Buffer( LayoutCodex::Resolve( std::move( lay ) ) )
	{}
	Buffer::Buffer( const CookedLayout& lay ) noxnd
		:
		pLayoutRoot( lay.ShareRoot() ),
		bytes( pLayoutRoot->GetOffsetEnd() )
	{}
	Buffer::Buffer( CookedLayout&& lay ) noxnd
		:
		pLayoutRoot( lay.RelinquishRoot() ),
		bytes( pLayoutRoot->GetOffsetEnd() )
	{}
	Buffer::Buffer( const Buffer& buf ) noexcept
		:
		pLayoutRoot( buf.pLayoutRoot ),
		bytes( buf.bytes )
	{}
	Buffer::Buffer( Buffer&& buf ) noexcept
		:
		pLayoutRoot( std::move( buf.pLayoutRoot ) ),
		bytes( std::move( buf.bytes ) )
	{}
	ElementRef Buffer::operator[]( const std::string& key ) noxnd
	{
		return { &(*pLayoutRoot)[key],bytes.data(),0u };
	}
	ConstElementRef Buffer::operator[]( const std::string& key ) const noxnd
	{
		return const_cast<Buffer&>(*this)[key];
	}
	const char* Buffer::GetData() const noexcept
	{
		return bytes.data();
	}
	size_t Buffer::GetSizeInBytes() const noexcept
	{
		return bytes.size();
	}
	const LayoutElement& Buffer::GetRootLayoutElement() const noexcept
	{
		return *pLayoutRoot;
	}
	void Buffer::CopyFrom( const Buffer& other ) noxnd
	{
		assert( &GetRootLayoutElement() == &other.GetRootLayoutElement() );
		std::copy( other.bytes.begin(),other.bytes.end(),bytes.begin() );
	}
	std::shared_ptr<LayoutElement> Buffer::ShareLayoutRoot() const noexcept
	{
		return pLayoutRoot;
	}
}
