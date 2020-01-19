#include "ScriptCommander.h"
#include <sstream>
#include <fstream>
#include <filesystem>
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
				else if( commandName == "publish" )
				{
					Publish( params.at( "dest" ) );
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

void ScriptCommander::Publish( std::string path ) const
{
	namespace fs = std::filesystem;
	fs::create_directory( path );
	// copy executable
	fs::copy_file( R"(..\x64\Release\hw3d.exe)",path + R"(\hw3d.exe)",fs::copy_options::overwrite_existing );
	// copy assimp ini
	fs::copy_file( "imgui_default.ini",path + R"(\imgui_default.ini)",fs::copy_options::overwrite_existing );
	// copy all dlls
	for( auto& p : fs::directory_iterator( "" ) )
	{
		if( p.path().extension() == L".dll" )
		{
			fs::copy_file( p.path(),path + "\\" + p.path().filename().string(),
				fs::copy_options::overwrite_existing
			);
		}
	}
	// copy compiled shaders
	fs::copy( "ShaderBins",path + R"(\ShaderBins)",fs::copy_options::overwrite_existing );
	// copy assets
	fs::copy( "Images",path + R"(\Images)",fs::copy_options::overwrite_existing | fs::copy_options::recursive );
	fs::copy( "Models",path + R"(\Models)",fs::copy_options::overwrite_existing | fs::copy_options::recursive );
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
