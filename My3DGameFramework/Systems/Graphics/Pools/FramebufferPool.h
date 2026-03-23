#pragma once

namespace MGF3D
{
	template<typename T>
	class FramebufferPool
	{
	public:
		FramebufferPool(uint32 maxCapacity = 50);
		virtual ~FramebufferPool();

	public:
		void Release(SharedPtr<T> fbo);
		void Clear();

	protected:
		SharedPtr<T> FindAvailable(usize hash);
		void Trim();

	protected:
		SMap<usize, SDeque<SharedPtr<T>>> m_available;
		uint32 m_totalCount		{ 0 };
		uint32 m_maxCapacity	{ 50 };
	};
}

#include "Pools/FramebufferPool.inl"