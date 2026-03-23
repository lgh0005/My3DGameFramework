#include "FramebufferPool.h"
#pragma once

namespace MGF3D
{
	template<typename T>
	inline FramebufferPool<T>::FramebufferPool(uint32 maxCapacity)
		: m_maxCapacity(maxCapacity) { }

	template<typename T>
	inline MGF3D::FramebufferPool<T>::~FramebufferPool()
	{
		Clear();
	}

	template<typename T>
	inline void FramebufferPool<T>::Release(SharedPtr<T> fbo)
	{
		if (fbo == nullptr) return;

		usize fboHash = fbo->GetHash();
		m_available[fboHash].PushBack(fbo);
		m_totalCount++;

		// 용량을 초과하면 가장 오래된(사용되지 않은) 프레임버퍼부터 메모리에서 해제
		Trim();
	}

	template<typename T>
	inline void FramebufferPool<T>::Clear()
	{
		m_available.Clear();
		m_totalCount = 0;
	}

	template<typename T>
	inline SharedPtr<T> FramebufferPool<T>::FindAvailable(usize hash)
	{
		// 이전에 상기시켜 주신 커스텀 맵의 Find(포인터 반환) 구조 적용
		auto pFboList = m_available.Find(hash);
		if (pFboList != nullptr && !pFboList->Empty())
		{
			// 가장 최근에 반납된 녀석을 다시 꺼내기 (MRU 방식이 캐시 효율에 유리)
			auto fbo = pFboList->Back();
			pFboList->PopBack();
			m_totalCount--;
			return fbo;
		}

		return nullptr;
	}

	template<typename T>
	inline void FramebufferPool<T>::Trim()
	{
		// 0. 전체 보관량이 최대 허용치를 넘었을 경우
		while (m_totalCount > m_maxCapacity)
		{
			bool removed = false;

			// 1. 삭제 대상 탐색 및 제거
			for (auto& available : m_available)
			{
				auto& pFboList = available.second;
				if (!pFboList.Empty())
				{
					// 가장 앞에 있는 녀석이 반납된 지 제일 오래된 대상
					pFboList.PopFront();
					m_totalCount--;
					removed = true;
					break;
				}
			}

			// 2. 무한 루프 방지 안전장치
			if (!removed)
			{
				m_totalCount = 0;
				break;
			}
		}
	}
}