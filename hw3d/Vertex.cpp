#define DVTX_SOURCE_FILE
#include "Vertex.h"

namespace Dvtx
{
	// VertexLayout
	const VertexLayout::Element& VertexLayout::ResolveByIndex( size_t i ) const noxnd
	{
		return elements[i];
	}
	VertexLayout& VertexLayout::Append( ElementType type ) noxnd
	{
		elements.emplace_back( type,Size() );
		return *this;
	}
	size_t VertexLayout::Size() const noxnd
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}
	size_t VertexLayout::GetElementCount() const noexcept
	{
		return elements.size();
	}
	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noxnd
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve( GetElementCount() );
		for( const auto& e : elements )
		{
			desc.push_back( e.GetDesc() );
		}
		return desc;
	}
	std::string VertexLayout::GetCode() const noxnd
	{
		std::string code;
		for( const auto& e : elements )
		{
			code += e.GetCode();
		}
		return code;
	}


	// VertexLayout::Element
	VertexLayout::Element::Element( ElementType type,size_t offset )
		:
		type( type ),
		offset( offset )
	{}
	size_t VertexLayout::Element::GetOffsetAfter() const noxnd
	{
		return offset + Size();
	}
	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}
	size_t VertexLayout::Element::Size() const noxnd
	{
		return SizeOf( type );
	}
	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return type;
	}
	
	template<VertexLayout::ElementType type>
	struct SysSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return sizeof( VertexLayout::Map<type>::SysType );
		}
	};
	constexpr size_t VertexLayout::Element::SizeOf( ElementType type ) noxnd
	{
		return Bridge<SysSizeLookup>( type );
	}

	template<VertexLayout::ElementType type>
	struct CodeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return VertexLayout::Map<type>::code;
		}
	};
	const char* Dvtx::VertexLayout::Element::GetCode() const noexcept
	{
		return Bridge<CodeLookup>( type );
	}

	template<VertexLayout::ElementType type> struct DescGenerate {
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec( size_t offset ) noexcept {
			return { 
				VertexLayout::Map<type>::semantic,0,
				VertexLayout::Map<type>::dxgiFormat,
				0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
			};
		}
	};
	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
	{
		return Bridge<DescGenerate>( type,GetOffset() );
	}


	// Vertex
	Vertex::Vertex( char* pData,const VertexLayout& layout ) noxnd
		:
		pData( pData ),
		layout( layout )
	{
		assert( pData != nullptr );
	}
	ConstVertex::ConstVertex( const Vertex& v ) noxnd
		:
		vertex( v )
	{}


	// VertexBuffer
	VertexBuffer::VertexBuffer( VertexLayout layout,size_t size ) noxnd
		:
		layout( std::move( layout ) )
	{
		Resize( size );
	}
	void VertexBuffer::Resize( size_t newSize ) noxnd
	{
		const auto size = Size();
		if( size < newSize )
		{
			buffer.resize( buffer.size() + layout.Size() * (newSize - size) );
		}
	}
	const char* VertexBuffer::GetData() const noxnd
	{
		return buffer.data();
	}
	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}
	size_t VertexBuffer::Size() const noxnd
	{
		return buffer.size() / layout.Size();
	}
	size_t VertexBuffer::SizeBytes() const noxnd
	{
		return buffer.size();
	}
	Vertex VertexBuffer::Back() noxnd
	{
		assert( buffer.size() != 0u );
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}
	Vertex VertexBuffer::Front() noxnd
	{
		assert( buffer.size() != 0u );
		return Vertex{ buffer.data(),layout };
	}
	Vertex VertexBuffer::operator[]( size_t i ) noxnd
	{
		assert( i < Size() );
		return Vertex{ buffer.data() + layout.Size() * i,layout };
	}
	ConstVertex VertexBuffer::Back() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}
	ConstVertex VertexBuffer::Front() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}
	ConstVertex VertexBuffer::operator[]( size_t i ) const noxnd
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}