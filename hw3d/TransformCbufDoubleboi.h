#pragma once
#include "TransformCbuf.h"

//namespace Bind
//{
//	class TransformCbufDoubleboi : public TransformCbuf
//	{
//	public:
//		TransformCbufDoubleboi( Graphics& gfx,const Drawable& parent,UINT slotV = 0u,UINT slotP = 0u );
//		void Bind( Graphics& gfx ) noexcept override;
//	protected:
//		void UpdateBindImpl( Graphics& gfx,const Transforms& tf ) noexcept;
//	private:
//		static std::unique_ptr<PixelConstantBuffer<Transforms>> pPcbuf;
//	};
//}