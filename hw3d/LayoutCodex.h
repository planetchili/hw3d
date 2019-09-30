#pragma once
#include "DynamicConstant.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

class LayoutCodex
{
public:
	static bool Has( const std::string& tag )
	{
		return Get_().Has_( tag );
	}
	static Dcb::Layout Load( const std::string& tag )
	{
		return Get_().Load_( tag );
	}
	static void Store( const std::string& tag,Dcb::Layout& layout )
	{
		Get_().Store_( tag,layout );
	}
private:
	static LayoutCodex& Get_()
	{
		static LayoutCodex codex;
		return codex;
	}
	bool Has_( const std::string& tag ) const
	{
		return map.find( tag ) != map.end();
	}
	Dcb::Layout Load_( const std::string& tag ) const
	{
		return { map.find( tag )->second };
	}
	void Store_( const std::string& tag,Dcb::Layout& layout )
	{
		auto r = map.insert( { tag,layout.Finalize() } );
		assert( r.second );
	}
private:
	std::unordered_map<std::string,std::shared_ptr<Dcb::LayoutElement>> map;
};