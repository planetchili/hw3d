#include "ModelException.h"
#include <sstream>

ModelException::ModelException( int line,const char* file,std::string note ) noexcept
	:
	ChiliException( line,file ),
	note( std::move( note ) )
{}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Chili Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}