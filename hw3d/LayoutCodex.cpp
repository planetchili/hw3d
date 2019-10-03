#include "LayoutCodex.h"


namespace Dcb
{
	Dcb::Layout LayoutCodex::Resolve( Dcb::Layout& layout ) noxnd
	{
		layout.Finalize();
		auto sig = layout.GetSignature();
		auto& map = Get_().map;
		const auto i = map.find( sig );
		// idential layout already exists
		if( i != map.end() )
		{
			return { i->second };
		}
		// add layout root element to map
		auto result = map.insert( { std::move( sig ),layout.ShareRoot() } );
		// return layout with additional reference to root
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get_() noexcept
	{
		static LayoutCodex codex;
		return codex;
	}
}