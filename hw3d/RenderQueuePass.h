#pragma once
#include "BindingPass.h"
#include "Job.h"
#include <vector>

namespace Rgph
{
	class RenderQueuePass : public BindingPass
	{
	public:
		using BindingPass::BindingPass;
		void Accept( Job job ) noexcept;
		void Execute( Graphics& gfx ) const noxnd override;
		void Reset() noxnd override;
	private:
		std::vector<Job> jobs;
	};
}