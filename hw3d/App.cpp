#include "App.h"

App::App()
	:
	wnd( 800,600,"The Donkey Fart Box" )
{}

int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	static std::string title;
	while( const auto e = wnd.kbd.ReadKey() )
	{
		if( e->IsPress() && e->GetCode() == VK_BACK )
		{
			title.clear();
			wnd.SetTitle( title );
		}
	}
	while( const auto c = wnd.kbd.ReadChar() )
	{
		if( *c != 0x8 ) // don't print backspace
		{
			title += *c;
			wnd.SetTitle( title );
		}
	}
	const float c = sin( timer.Peek() ) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer( c,c,1.0f );
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();
}