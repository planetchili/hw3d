/******************************************************************************************
*	Chili Direct3D Engine																  *
*	Copyright 2018 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of Chili Direct3D Engine.											  *
*																						  *
*	Chili Direct3D Engine is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili Direct3D Engine is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili Direct3D Engine.  If not, see <http://www.gnu.org/licenses/>.    *
******************************************************************************************/
#include "Keyboard.h"

bool Keyboard::KeyIsPressed( unsigned char keycode ) const noexcept
{
	return keystates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
	if( keybuffer.size() > 0u )
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return {};
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

std::optional<char> Keyboard::ReadChar() noexcept
{
	if( charbuffer.size() > 0u )
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	return {};
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charbuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	keybuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	charbuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed( unsigned char keycode ) noexcept
{
	keystates[keycode] = true;
	keybuffer.push( Keyboard::Event( Keyboard::Event::Type::Press,keycode ) );
	TrimBuffer( keybuffer );
}

void Keyboard::OnKeyReleased( unsigned char keycode ) noexcept
{
	keystates[keycode] = false;
	keybuffer.push( Keyboard::Event( Keyboard::Event::Type::Release,keycode ) );
	TrimBuffer( keybuffer );
}

void Keyboard::OnChar( char character ) noexcept
{
	charbuffer.push( character );
	TrimBuffer( charbuffer );
}

void Keyboard::ClearState() noexcept
{
	keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer( std::queue<T>& buffer ) noexcept
{
	while( buffer.size() > bufferSize )
	{
		buffer.pop();
	}
}

