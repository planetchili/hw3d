#pragma once
#include <string>
#include <vector>
#include <memory>
#include "ConditionalNoexcept.h"

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace Rgph
{
	class Pass;
	class RenderQueuePass;
	class PassOutput;
	class PassInput;

	class RenderGraph
	{
	public:
		RenderGraph( Graphics& gfx );
		~RenderGraph();
		void Execute( Graphics& gfx ) noxnd;
		void Reset() noexcept;
		RenderQueuePass& GetRenderQueue( const std::string& passName );
	protected:
		void SetSinkTarget( const std::string& sinkName,const std::string& target );
		void AddGlobalSource( std::unique_ptr<PassOutput> );
		void AddGlobalSink( std::unique_ptr<PassInput> );
		void Finalize();
		void AppendPass( std::unique_ptr<Pass> pass );
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
}