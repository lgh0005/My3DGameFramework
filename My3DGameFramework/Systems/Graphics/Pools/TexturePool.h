#pragma once

namespace MGF3D
{
	template <typename T>
	class TexturePool
	{
	public:
		TexturePool(uint32 maxCapacity = 100);
		virtual ~TexturePool();

	public:
		void Release(SharedPtr<T> texture);
		void Clear();

	protected:
		SharedPtr<T> FindAvailable(usize textureHash);
		void Trim();
		
	protected:
		SMap<usize, SDeque<SharedPtr<T>>> m_available;
		uint32 m_totalCount		{ 0 };

		// TODO: 이건 어쩌면 TextureManager로부터 받아와야 할 수 있음
		// ex) 하드웨어가 동시에 바인딩 가능한 텍스쳐 수의 1/3을 각 풀마다
		// 두거나 아니면 2D가 훨씬 더 빈번하게 사용될테니 비율을 조정하거나.
		uint32 m_maxCapacity	{ 100 };
	};
}

#include "Pools/TexturePool.inl"