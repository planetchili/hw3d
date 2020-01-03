#pragma once
#include "ChiliException.h"

#define RGC_EXCEPTION( message ) RenderGraphCompileException( (message),__LINE__,__FILE__ )

class RenderGraphCompileException : public ChiliException
{
public:
	RenderGraphCompileException( std::string message,int line,const char* file ) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetMessage() const noexcept;
private:
	std::string message;
};