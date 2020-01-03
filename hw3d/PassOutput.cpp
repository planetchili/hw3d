#include "PassOutput.h"
#include "RenderGraphCompileException.h"
#include <algorithm>
#include <cctype>


PassOutput::PassOutput( std::string nameIn )
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

std::shared_ptr<Bind::Bindable> PassOutput::YieldImmutable()
{
	throw RGC_EXCEPTION( "Output cannot be accessed as immutable" );
}

std::shared_ptr<Bind::BufferResource> PassOutput::YieldBuffer()
{
	throw RGC_EXCEPTION( "Output cannot be accessed as buffer" );
}

const std::string& PassOutput::GetName() const noexcept
{
	return name;
}