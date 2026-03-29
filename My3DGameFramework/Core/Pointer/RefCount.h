#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	class RefCount : public PoolAlloc
	{
		MGF_DISABLE_COPY(RefCount)

	protected:
		RefCount();
		virtual ~RefCount();

	public:
		void IncRef();
		void DecRef();
		int32 GetRefCount();

	private:
		mutable Atomic<int32> m_refCount  { 0 };
	};
}