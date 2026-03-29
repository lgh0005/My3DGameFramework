#include "CorePch.h"
#include "RefCount.h"

namespace MGF3D
{
	RefCount::RefCount() = default;
	RefCount::~RefCount() = default;

	void RefCount::IncRef()
	{
		m_refCount.fetch_add(1, std::memory_order_relaxed);
	}

	void RefCount::DecRef()
	{
		if (m_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			delete this;
	}

	int32 RefCount::GetRefCount()
	{
		return m_refCount.load(std::memory_order_relaxed);
	}
}