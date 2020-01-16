#include "Source.h"
#include "RenderGraphCompileException.h"
#include <algorithm>
#include <cctype>

namespace Rgph
{
	Source::Source( std::string nameIn )
		:
		name( std::move( nameIn ) )
	{
		if( name.empty() )
		{
			throw RGC_EXCEPTION( "Empty output name" );
		}
		const bool nameCharsValid = std::all_of( name.begin(),name.end(),[]( char c ) {
			return std::isalnum( c ) || c == '_';
		} );
		if( !nameCharsValid || std::isdigit( name.front() ) )
		{
			throw RGC_EXCEPTION( "Invalid output name: " + name );
		}
	}

	std::shared_ptr<Bind::Bindable> Source::YieldBindable()
	{
		throw RGC_EXCEPTION( "Output cannot be accessed as bindable" );
	}

	std::shared_ptr<Bind::BufferResource> Source::YieldBuffer()
	{
		throw RGC_EXCEPTION( "Output cannot be accessed as buffer" );
	}

	const std::string& Source::GetName() const noexcept
	{
		return name;
	}
}