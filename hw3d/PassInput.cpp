#include "PassInput.h"
#include "PassOutput.h"
#include "RenderGraphCompileException.h"
#include "ChiliUtil.h"
#include "Bindable.h"
#include "BufferResource.h"
#include <sstream>
#include <cctype>

namespace Rgph
{
	PassInput::PassInput( std::string registeredNameIn )
		:
		registeredName( std::move( registeredNameIn ) )
	{
		if( registeredName.empty() )
		{
			throw RGC_EXCEPTION( "Empty output name" );
		}
		const bool nameCharsValid = std::all_of( registeredName.begin(),registeredName.end(),[]( char c ) {
			return std::isalnum( c ) || c == '_';
		} );
		if( !nameCharsValid || std::isdigit( registeredName.front() ) )
		{
			throw RGC_EXCEPTION( "Invalid output name: " + registeredName );
		}
	}

	const std::string& PassInput::GetRegisteredName() const noexcept
	{
		return registeredName;
	}

	const std::string& PassInput::GetPassName() const noexcept
	{
		return passName;
	}

	const std::string& PassInput::GetOutputName() const noexcept
	{
		return outputName;
	}

	void PassInput::SetTarget( std::string passName,std::string outputName )
	{
		{
			if( passName.empty() )
			{
				throw RGC_EXCEPTION( "Empty output name" );
			}
			const bool nameCharsValid = std::all_of( passName.begin(),passName.end(),[]( char c ) {
				return std::isalnum( c ) || c == '_';
			} );
			if( passName != "$" && (!nameCharsValid || std::isdigit( passName.front() )) )
			{
				throw RGC_EXCEPTION( "Invalid output name: " + registeredName );
			}
			this->passName = passName;
		}
		{
			if( outputName.empty() )
			{
				throw RGC_EXCEPTION( "Empty output name" );
			}
			const bool nameCharsValid = std::all_of( outputName.begin(),outputName.end(),[]( char c ) {
				return std::isalnum( c ) || c == '_';
			} );
			if( !nameCharsValid || std::isdigit( outputName.front() ) )
			{
				throw RGC_EXCEPTION( "Invalid output name: " + registeredName );
			}
			this->outputName = outputName;
		}
	}
}
