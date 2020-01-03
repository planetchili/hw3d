#pragma once
#include <string>
#include <vector>
#include <memory>
#include "ConditionalNoexcept.h"

class Pass;
class RenderQueuePass;
class PassOutput;
class PassInput;
class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

class RenderGraph
{
public:
	RenderGraph( Graphics& gfx );
	~RenderGraph();
	void SetSinkTarget( const std::string& sinkName,const std::string& target );
	void AppendPass( std::unique_ptr<Pass> pass );
	void Execute( Graphics& gfx ) noxnd;
	void Reset() noexcept;
	void Finalize();
	RenderQueuePass& GetRenderQueue( const std::string& passName );
private:
	void LinkPassInputs( Pass& pass );
	void LinkGlobalSinks();
private:
	std::vector<std::unique_ptr<Pass>> passes;
	std::vector<std::unique_ptr<PassOutput>> globalSources;
	std::vector<std::unique_ptr<PassInput>> globalSinks;
	std::shared_ptr<Bind::RenderTarget> backBufferTarget;
	std::shared_ptr<Bind::DepthStencil> masterDepth;
	bool finalized = false;
};