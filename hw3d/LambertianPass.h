#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PassInput.h"
#include "PassOutput.h"

class Graphics;

class LambertianPass : public RenderQueuePass
{
public:
	LambertianPass( std::string name )
		:
		RenderQueuePass( std::move( name ) )
	{
		RegisterInput( BufferInput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
		RegisterInput( BufferInput<Bind::DepthStencil>::Make( "depthStencil",depthStencil ) );
		RegisterOutput( BufferOutput<Bind::RenderTarget>::Make( "renderTarget",renderTarget ) );
	}
private:
	std::vector<Job> jobs;
};