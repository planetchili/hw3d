#include "ScriptCommander.h"
#include <sstream>
#include <fstream>
#include "json.hpp"
#include "TexturePreprocessor.h"

namespace jso = nlohmann;
using namespace std::string_literals;

#define SCRIPT_ERROR( msg ) Exception( __LINE__,__FILE__,scriptPath,(msg) )

ScriptCommander::ScriptCommander( const std::vector<std::string>& args )
{
	if( args.size() >= 2 && args[0] == "--commands" )
	{
		const auto scriptPath = args[1];
		std::ifstream script( scriptPath );
		if( !script.is_open() )
		{
			throw SCRIPT_ERROR( "Unable to open script file"s );
		}
		jso::json top;
		script >> top;

		if( top.at( "enabled" ) )
		{
			bool abort = false;
			for( const auto& j : top.at("commands") )
			{
				const auto commandName = j.at( "command" ).get<std::string>();
				const auto params = j.at( "params" );
				if( commandName == "flip-y" )
				{
					const auto source = params.at( "source" );
					TexturePreprocessor::FlipYNormalMap( source,params.value( "dest",source ) );
					abort = true;
				}
				else if( commandName == "flip-y-obj" )
				{
					TexturePreprocessor::FlipYAllNormalMapsInObj( params.at( "source" ) );
					abort = true;
				}
				else if( commandName == "validate-nmap" )
				{
					TexturePreprocessor::ValidateNormalMap( params.at( "source" ),params.at( "min" ),params.at( "max" ) );
					abort = true;
				}
				else if( commandName == "make-stripes" )
				{
					TexturePreprocessor::MakeStripes( params.at( "dest" ),params.at( "size" ),params.at( "stripeWidth" ) );
					abort = true;
				}
				else
				{
					throw SCRIPT_ERROR( "Unknown command: "s + commandName );
				}
			}
			if( abort )
			{
				throw Completion( "Command(s) completed successfully" );
			}
		}
	}
}



ScriptCommander::Completion::Completion( const std::string& content ) noexcept
	:
	ChiliException( 69,"@ScriptCommanderAbort" ),
	content( content )
{}

const char* ScriptCommander::Completion::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << std::endl << content;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ScriptCommander::Completion::GetType() const noexcept
{
	return "Script Command Completed";
}


ScriptCommander::Exception::Exception( int line,const char* file,const std::string& script,const std::string& message ) noexcept
	:
	ChiliException( 69,"@ScriptCommanderAbort" ),
	script( script ),
	message( message )
{}

const char* ScriptCommander::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Script File] " << script << std::endl
		<< message;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ScriptCommander::Exception::GetType() const noexcept
{
	return "Script Command Error";
}
