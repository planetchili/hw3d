#include "DynamicConstant.h"
#include "LayoutCodex.h"
#include <cstring>

namespace dx = DirectX;

void TestDynamicConstant()
{
	using namespace std::string_literals;
	// data roundtrip tests
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Struct>( "butts"s );
		s["butts"s].Add<Dcb::Float3>( "pubes"s );
		s["butts"s].Add<Dcb::Float>( "dank"s );
		s.Add<Dcb::Float>( "woot"s );
		s.Add<Dcb::Array>( "arr"s );
		s["arr"s].Set<Dcb::Struct>( 4 );
		s["arr"s].T().Add<Dcb::Float3>( "twerk"s );
		s["arr"s].T().Add<Dcb::Array>( "werk"s );
		s["arr"s].T()["werk"s].Set<Dcb::Float>( 6 );
		s["arr"s].T().Add<Dcb::Array>( "meta"s );
		s["arr"s].T()["meta"s].Set<Dcb::Array>( 6 );
		s["arr"s].T()["meta"s].T().Set<Dcb::Matrix>( 4 );
		s["arr"s].T().Add<Dcb::Bool>( "booler" );

		// fails: duplicate symbol name
		// s.Add<Dcb::Bool>( "arr"s );

		// fails: bad symbol name
		//s.Add<Dcb::Bool>( "69man" );

		auto b = Dcb::Buffer( std::move( s ) );

		const auto sig = b.GetRootLayoutElement().GetSignature();


		{
			auto exp = 42.0f;
			b["woot"s] = exp;
			float act = b["woot"s];
			assert( act == exp );
		}
		{
			auto exp = 420.0f;
			b["butts"s]["dank"s] = exp;
			float act = b["butts"s]["dank"s];
			assert( act == exp );
		}
		{
			auto exp = 111.0f;
			b["arr"s][2]["werk"s][5] = exp;
			float act = b["arr"s][2]["werk"s][5];
			assert( act == exp );
		}
		{
			auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
			b["butts"s]["pubes"s] = exp;
			dx::XMFLOAT3 act = b["butts"s]["pubes"s];
			assert( !std::memcmp( &exp,&act,sizeof( DirectX::XMFLOAT3 ) ) );
		}
		{
			DirectX::XMFLOAT4X4 exp;
			dx::XMStoreFloat4x4(
				&exp,
				dx::XMMatrixIdentity()
			);
			b["arr"s][2]["meta"s][5][3] = exp;
			dx::XMFLOAT4X4 act = b["arr"s][2]["meta"s][5][3];
			assert( !std::memcmp( &exp,&act,sizeof( DirectX::XMFLOAT4X4 ) ) );
		}
		{
			auto exp = true;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert( act == exp );
		}
		{
			auto exp = false;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert( act == exp );
		}
		// exists
		{
			assert( b["butts"s]["pubes"s].Exists() );
			assert( !b["butts"s]["fubar"s].Exists() );
			if( auto ref = b["butts"s]["pubes"s]; ref.Exists() )
			{
				dx::XMFLOAT3 f = ref;
				assert( f.x == 69.0f );
			}
		}

		const auto& cb = b;
		{
			dx::XMFLOAT4X4 act = cb["arr"s][2]["meta"s][5][3];
			assert( act._11 == 1.0f );
		}
		// this doesn't compile: buffer is const
		// cb["arr"][2]["booler"] = true;

		// this fails assertion: array out of bounds
		// cb["arr"s][200];

	}
	// size test array of arrays
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Array>( 6 );
		s["arr"].T().Set<Dcb::Matrix>( 4 );
		auto b = Dcb::Buffer( std::move( s ) );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 4u * 4u * 6u );
	}
	// size test array of structs with padding
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Struct>( 6 );
		s["arr"s].T().Add<Dcb::Float2>( "a" );
		s["arr"].T().Add<Dcb::Float3>( "b"s );
		auto b = Dcb::Buffer( std::move( s ) );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 2u * 6u );
	}
	// size test array of primitive that needs padding
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Float3>( 6 );
		auto b = Dcb::Buffer( std::move( s ) );

		auto act = b.GetSizeInBytes();
		assert( act == 16u * 6u );
	}
	// testing CookedLayout
	{
		Dcb::RawLayout s;
		s.Add<Dcb::Array>( "arr" );
		s["arr"].Set<Dcb::Float3>( 6 );
		auto cooked = Dcb::LayoutCodex::Resolve( std::move( s ) );
		// raw is cleared after donating
		s.Add<Dcb::Float>( "arr" );
		// fails to compile, cooked returns const&
		// cooked["arr"].Add<Dcb::Float>("buttman");
		auto b1 = Dcb::Buffer( cooked );
		b1["arr"][0] = dx::XMFLOAT3{ 69.0f,0.0f,0.0f };
		auto b2 = Dcb::Buffer( cooked );
		b2["arr"][0] = dx::XMFLOAT3{ 420.0f,0.0f,0.0f };
		assert( static_cast<dx::XMFLOAT3>(b1["arr"][0]).x == 69.0f );
		assert( static_cast<dx::XMFLOAT3>(b2["arr"][0]).x == 420.0f );
	}
}