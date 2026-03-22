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
		void IncRef() const;
		void DecRef() const;
		int32 GetRefCount() const;

	private:
		mutable Atomic<int32> m_refCount  { 0 };
	};
}