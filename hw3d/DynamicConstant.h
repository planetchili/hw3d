#pragma once
#include "ConditionalNoexcept.h"
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>

// macro simplifies adding base virtual resolution function to LayoutElement
// resolution functions are the system by which polymorphic type accesses are validated
// the bases here all provide a default behavior of failing assertion, then for example
// Float1 will overload ResolveFloat1() and provide an implementation that doesn't fail
#define DCB_RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noxnd;

// this allows metaclass templating for leaf layout types like Float1, Bool, etc.
// macro is used so that the names of the overloaded ResolveXXX themselves can be
// generated automatically. See LayoutElement base class for function meanings
#define DCB_LEAF_ELEMENT_IMPL(eltype,systype,hlslSize) \
class eltype : public LayoutElement \
{ \
	friend LayoutElement; \
public: \
	using SystemType = systype; \
	size_t Resolve ## eltype() const noxnd final;\
	size_t GetOffsetEnd() const noexcept final;\
	std::string GetSignature() const noxnd final; \
protected: \
	size_t Finalize( size_t offset_in ) noxnd final;\
	size_t ComputeSize() const noxnd final;\
};
#define DCB_LEAF_ELEMENT(eltype,systype) DCB_LEAF_ELEMENT_IMPL(eltype,systype,sizeof(systype))

// these macros serve to simplify declaration of all the conversions / assignments
// that need to be declared for the shell reference types (ElementRef / ElementRef::Ptr)
#define DCB_REF_CONVERSION(eltype,...) \
operator __VA_ARGS__ eltype::SystemType&() noxnd;
#define DCB_REF_ASSIGN(eltype) \
eltype::SystemType& operator=( const eltype::SystemType& rhs ) noxnd;
#define DCB_REF_NONCONST(eltype) DCB_REF_CONVERSION(eltype) DCB_REF_ASSIGN(eltype)
#define DCB_REF_CONST(eltype) DCB_REF_CONVERSION(eltype,const)

#define DCB_PTR_CONVERSION(eltype,...) \
operator __VA_ARGS__ eltype::SystemType*() noxnd;



namespace Dcb
{
	namespace dx = DirectX;
	
	// this abstract class is the base of the Layout system that describes the structure
	// of a dynamic constant buffer. They layout is a kind of tree structure LayoutElements
	class LayoutElement
	{
		// friend relationships are used liberally throught the DynamicConstant system
		// instead of seeing the various classes in this system as encapsulated decoupled
		// units, they must be viewed as aspect of one large monolithic system
		// the reason for the friend relationships is generally so that intermediate
		// classes that the client should not create can have their constructors made
		// private, Finalize() cannot be called on arbitrary LayoutElements, etc.
		friend class RawLayout;
		friend class Array;
		friend class Struct;
	public:
		virtual ~LayoutElement();

		// get a string signature for this element (recursive)
		virtual std::string GetSignature() const noxnd = 0;
		// Check if element is "real"
		virtual bool Exists() const noexcept;
		// [] only works for Structs; access member (child node in tree) by name
		virtual LayoutElement& operator[]( const std::string& ) noxnd;
		const LayoutElement& operator[]( const std::string& key ) const noxnd;
		// T() only works for Arrays; gets the array type layout object
		// needed to further configure an array's type
		virtual LayoutElement& T() noxnd;
		const LayoutElement& T() const noxnd;

		// offset based- functions only work after finalization!
		size_t GetOffsetBegin() const noexcept;
		virtual size_t GetOffsetEnd() const noexcept = 0;
		// get size in bytes derived from offsets
		size_t GetSizeInBytes() const noexcept;

		// only works for Structs; add LayoutElement to struct
		template<typename T>
		LayoutElement& Add( const std::string& key ) noxnd;
		// only works for Arrays; set the type and the # of elements
		template<typename T>
		LayoutElement& Set( size_t size ) noxnd;

		// returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
		static size_t GetNextBoundaryOffset( size_t offset ) noexcept;

		// these are declarations virtual functions that the various leaf LayoutElement types
		// will override for the purposes of runtime checking whether a type conversion is allowed
		DCB_RESOLVE_BASE(Matrix)
		DCB_RESOLVE_BASE(Float4)
		DCB_RESOLVE_BASE(Float3)
		DCB_RESOLVE_BASE(Float2)
		DCB_RESOLVE_BASE(Float)
		DCB_RESOLVE_BASE(Bool)
	protected:
		// sets all offsets for element and subelements, returns offset directly after this element
		virtual size_t Finalize( size_t offset ) noxnd = 0;
		// computes the size of this element in bytes, considering padding on Arrays and Structs
		virtual size_t ComputeSize() const noxnd = 0;
	protected:
		// each element stores its own offset. this makes lookup to find its position in the byte buffer
		// fast. Special handling is required for situations where arrays are involved
		size_t offset = 0u;
	};


	// declarations of the Leaf types, see macro at top
	DCB_LEAF_ELEMENT( Matrix,dx::XMFLOAT4X4 )
	DCB_LEAF_ELEMENT( Float4,dx::XMFLOAT4 )
	DCB_LEAF_ELEMENT( Float3,dx::XMFLOAT3 )
	DCB_LEAF_ELEMENT( Float2,dx::XMFLOAT2 )
	DCB_LEAF_ELEMENT( Float,float )
	// Bool is special case, because its size on GPU does not match CPU size
	DCB_LEAF_ELEMENT_IMPL( Bool,bool,4u )

	// Struct is the core concrete layout type, is essentially of map of
	// string => LayoutElement. See LayoutElement for meanings of functions
	class Struct : public LayoutElement
	{
		friend LayoutElement;
	public:
		LayoutElement& operator[]( const std::string& key ) noxnd final;
		size_t GetOffsetEnd() const noexcept final;
		std::string GetSignature() const noxnd final;
		void Add( const std::string& name,std::unique_ptr<LayoutElement> pElement ) noxnd;
	protected:
		// client should not construct elements directly
		Struct() = default;
		size_t Finalize( size_t offset_in ) noxnd final;
		size_t ComputeSize() const noxnd final;
	private:
		// function to calculate padding according to the rules of HLSL structure packing
		static size_t CalculatePaddingBeforeElement( size_t offset,size_t size ) noexcept;
	private:
		// both map and vector are maintained for fast lookup + ordering
		// elements should be ordered in the order they are added to struct
		std::unordered_map<std::string,LayoutElement*> map;
		std::vector<std::unique_ptr<LayoutElement>> elements;
	};

	// an Array is much like a C-array. Indexing by integer rather than string, all
	// elements must be same type. because each array element does not have its own
	// LayoutElement, special processing is required for calculating offsets. An array
	// offset is added to the offsets stored in the LayoutElement. Nested arrays add their offsets
	class Array : public LayoutElement
	{
		friend LayoutElement;
	public:
		size_t GetOffsetEnd() const noexcept final;
		void Set( std::unique_ptr<LayoutElement> pElement,size_t size_in ) noxnd;
		LayoutElement& T() noxnd final;
		const LayoutElement& T() const noxnd;
		std::string GetSignature() const noxnd final;
		bool IndexInBounds( size_t index ) const noexcept;
	protected:
		// client should not construct elements directly
		Array() = default;
		size_t Finalize( size_t offset_in ) noxnd final;
		size_t ComputeSize() const noxnd final;
	private:
		size_t size = 0u;
		std::unique_ptr<LayoutElement> pElement;
	};

	

	// the layout class serves as a shell to hold the root of the LayoutElement tree
	// client does not create LayoutElements directly, create a raw layout and then
	// use it to access the elements and add on from there. When building is done,
	// raw layout is moved to Codex (usually via Buffer::Make), and the internal layout
	// element tree is "delivered" (finalized and moved out). Codex returns a baked
	// layout, which the buffer can then use to initialize itself. Baked layout can
	// also be used to directly init multiple Buffers. baked layouts are conceptually
	// immutable. base Layout cannot be constructed.
	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const noxnd;
	protected:
		Layout() noexcept;
		Layout( std::shared_ptr<LayoutElement> pRoot ) noexcept;
		std::shared_ptr<LayoutElement> pRoot;
	};
	
	// Raw layout represents a layout that has not yet been finalized and registered
	// structure can be edited by adding layout nodes
	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout() = default;
		LayoutElement& operator[]( const std::string& key ) noxnd;
		template<typename T>
		LayoutElement& Add( const std::string& key ) noxnd
		{
			return pRoot->Add<T>( key );
		}
	private:
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
		void ClearRoot() noexcept;
	};
	
	// CookedLayout represend a completed and registered Layout shell object
	// layout tree is fixed
	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[]( const std::string& key ) const noxnd;
	private:
		// this ctor used by Codex to return cooked layouts
		CookedLayout( std::shared_ptr<LayoutElement> pRoot ) noexcept;
		// used by buffer to add reference to shared ptr to layout tree root
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	};



	// The reference classes (ElementRef and ConstElementRef) form the shells for
	// interfacing with a Buffer. Operations such as indexing [] return further
	// Ref objects. Ref objects overload assignment and conversion, so they can
	// be used in expressions. Typechecking is performed at runtime (via the
	// ResolveXXX virtual methods in the LayoutElements)
	class ConstElementRef
	{
		friend class ElementRef;
		friend class Buffer;
	public:
		// a Ref can be further operated on by unary& to return a Ptr shell
		// Ptr shell objects convert to a ptr to the underlying type
		class Ptr
		{
			friend ConstElementRef;
		public:
			DCB_PTR_CONVERSION( Matrix,const )
			DCB_PTR_CONVERSION( Float4,const )
			DCB_PTR_CONVERSION( Float3,const )
			DCB_PTR_CONVERSION( Float2,const )
			DCB_PTR_CONVERSION( Float,const )
			DCB_PTR_CONVERSION( Bool,const )
		private:
			// ptr should only be constructable from a ref
			Ptr( ConstElementRef& ref ) noexcept;
			ConstElementRef& ref;
		};
	public:
		// polymorphic function returns true for all node types instead of
		// EmptyLayout (see EmptyLayout class internal to DynamicConstant.cpp)
		bool Exists() const noexcept;
		ConstElementRef operator[]( const std::string& key ) noxnd;
		ConstElementRef operator[]( size_t index ) noxnd;
		Ptr operator&() noxnd;

		DCB_REF_CONST( Matrix )
		DCB_REF_CONST( Float4 )
		DCB_REF_CONST( Float3 )
		DCB_REF_CONST( Float2 )
		DCB_REF_CONST( Float )
		DCB_REF_CONST( Bool )
	private:
		// refs should only be constructable by other refs or by the buffer
		ConstElementRef( const LayoutElement* pLayout,char* pBytes,size_t offset ) noexcept;
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};

	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend ElementRef;
		public:
			DCB_PTR_CONVERSION( Matrix )
			DCB_PTR_CONVERSION( Float4 )
			DCB_PTR_CONVERSION( Float3 )
			DCB_PTR_CONVERSION( Float2 )
			DCB_PTR_CONVERSION( Float )
			DCB_PTR_CONVERSION( Bool )
		private:
			// ptr should only be constructable from a ref
			Ptr( ElementRef& ref ) noexcept;
			ElementRef& ref;
		};
	public:
		operator ConstElementRef() const noexcept;
		// polymorphic function returns true for all node types instead of
		// EmptyLayout (see EmptyLayout class internal to DynamicConstant.cpp)
		bool Exists() const noexcept;
		ElementRef operator[]( const std::string& key ) noxnd;
		ElementRef operator[]( size_t index ) noxnd;
		Ptr operator&() noxnd;

		DCB_REF_NONCONST(Matrix)
		DCB_REF_NONCONST(Float4)
		DCB_REF_NONCONST(Float3)
		DCB_REF_NONCONST(Float2)
		DCB_REF_NONCONST(Float)
		DCB_REF_NONCONST(Bool)
	private:
		// refs should only be constructable by other refs or by the buffer
		ElementRef( const LayoutElement* pLayout,char* pBytes,size_t offset ) noexcept;
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};


	// The buffer object is a combination of a raw byte buffer with a LayoutElement
	// tree structure which acts as an view/interpretation/overlay for those bytes
	// operator [] indexes into the root Struct, returning a Ref shell that can be
	// used to further index if struct/array, returning further Ref shells, or used
	// to access the data stored in the buffer if a Leaf element type
	class Buffer
	{
	public:
		// ctors private, clients call Make to create buffers
		// Make with a rawlayout first passes layout to Codex for cooking/Resolution
		static Buffer Make( RawLayout&& lay ) noxnd;
		static Buffer Make( const CookedLayout& lay ) noxnd;
		ElementRef operator[]( const std::string& key ) noxnd;
		ConstElementRef operator[]( const std::string& key ) const noxnd;
		const char* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetLayout() const noexcept;
		std::shared_ptr<LayoutElement> ShareLayout() const noexcept;
	private:
		Buffer( const CookedLayout& lay ) noexcept;
		std::shared_ptr<LayoutElement> pLayout;
		std::vector<char> bytes;
	};
	

	// template definition must be declared after Struct/Array have been defined
	// but still need to be present in header because: templates
	template<typename T>
	LayoutElement& LayoutElement::Add( const std::string& key ) noxnd
	{
		auto ps = dynamic_cast<Struct*>(this);
		assert( ps != nullptr );
		// need to allow make_unique access to the ctor
		struct Enabler : public T{};
		ps->Add( key,std::make_unique<Enabler>() );
		return *this;
	}

	template<typename T>
	LayoutElement& LayoutElement::Set( size_t size ) noxnd
	{
		auto pa = dynamic_cast<Array*>(this);
		assert( pa != nullptr );
		// need to allow make_unique access to the ctor
		struct Enabler : public T{};
		pa->Set( std::make_unique<Enabler>(),size );
		return *this;
	}
}

#undef DCB_RESOLVE_BASE
#undef DCB_LEAF_ELEMENT_IMPL
#undef DCB_LEAF_ELEMENT
#undef DCB_REF_CONVERSION
#undef DCB_REF_ASSIGN
#undef DCB_REF_NONCONST
#undef DCB_REF_CONST
#undef DCB_PTR_CONVERSION
