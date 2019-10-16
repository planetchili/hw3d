#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "ChiliTimer.h"

class PerfLog
{
private:
	struct Entry
	{
		Entry( std::string s,float t )
			:
			label( std::move( s ) ),
			time( t )
		{}
		std::string label;
		float time;
		void WriteTo( std::ostream& out ) const noexcept
		{
			if( label.empty() )
			{
				out << time * 1000.0f << "ms\n";
			}
			else
			{
				out << "[" << label << "] " << time * 1000.0f << "ms\n";
			}
		}
	};
	static PerfLog& Get_() noexcept
	{
		static PerfLog log;
		return log;
	}
	PerfLog() noexcept
	{
		entries.reserve( 3000 );
	}
	~PerfLog()
	{
		Flush_();
	}
	void Start_( const std::string& label = "" ) noexcept
	{
		entries.emplace_back( label,0.0f );
		timer.Mark();
	}
	void Mark_( const std::string& label = "" ) noexcept
	{
		float t = timer.Peek();
		entries.emplace_back( label,t );
	}
	void Flush_()
	{
		std::ofstream file( "perf.txt" );
		file << std::setprecision( 3 ) << std::fixed;
		for( const auto& e : entries )
		{
			e.WriteTo( file );
		}
	}
public:
	static void Start( const std::string& label = "" ) noexcept
	{
		Get_().Start_( label );
	}
	static void Mark( const std::string& label = "" ) noexcept
	{
		Get_().Mark_( label );
	}
private:
	ChiliTimer timer;
	std::vector<Entry> entries;
};