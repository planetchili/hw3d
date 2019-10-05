#pragma once
#include "DynamicConstant.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace Dcb
{
	class LayoutCodex
	{
	public:
		static Dcb::CookedLayout Resolve( Dcb::RawLayout&& layout ) noxnd;
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string,std::shared_ptr<Dcb::LayoutElement>> map;
	};
}