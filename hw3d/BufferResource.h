#pragma once

class Graphics;

namespace Bind
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;
		virtual void BindAsBuffer( Graphics& ) noexcept = 0;
		virtual void BindAsBuffer( Graphics&,BufferResource* ) noexcept = 0;
		virtual void Clear( Graphics& ) noexcept = 0;
	};
}