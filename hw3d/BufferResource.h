#pragma once
#include "ConditionalNoexcept.h"

class Graphics;

namespace Bind
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;
		virtual void BindAsBuffer( Graphics& ) noxnd = 0;
		virtual void BindAsBuffer( Graphics&,BufferResource* ) noxnd = 0;
		virtual void Clear( Graphics& ) noxnd = 0;
	};
}