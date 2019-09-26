#pragma once
#include <vector>
#include <string>
#include "ChiliException.h"

class ScriptCommander
{
public:
	class Completion : public ChiliException
	{
	public:
		Completion( const std::string& content ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
	private:
		std::string content;
	};
	class Exception : public ChiliException
	{
	public:
		Exception( int line,const char* file,const std::string& script = "",const std::string& message = "" ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
	private:
		std::string script;
		std::string message;
	};
public:
	ScriptCommander( const std::vector<std::string>& args );
};