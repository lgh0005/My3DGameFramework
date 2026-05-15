#pragma once
#include "Sources/NamedResource.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	template<typename T, typename... Args>
	std::shared_ptr<T> ResourceManager::Create(Args&&... args)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Resource, T>,
			"T must inherit from Resource"
		);

		auto newResource = T::Create(std::forward<Args>(args)...);
		if (newResource) RegisterSync(newResource);
		return newResource;
	}

	template<typename T, typename... Args>
	std::shared_ptr<T> ResourceManager::CreateImmediate(Args&&... args)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Resource, T>,
			"T must inherit from Resource"
		);

		auto newResource = T::Create(std::forward<Args>(args)...);
		if (newResource)
		{
			// RegisterSync를 호출하지 않고 그 자리에서 즉시 할당을 시도합니다.
			if (newResource->OnSyncCreate()) newResource->SetState(EResourceState::Ready);
			else
			{
				newResource->SetState(EResourceState::Failed);
				return nullptr;
			}
		}

		return newResource;
	}

	template<typename T, typename... Args>
	std::shared_ptr<T> ResourceManager::GetOrCreate(const String& name, Args&&... args)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<NamedResource, T>,
			"T must inherit from NamedResource"
		);

		StringHash hash(name);

		// 1. 첫 번째 캐시 검사
		{
			LockScope lock(m_mutex);
			auto it = m_namedCache.find(hash);
			if (it != m_namedCache.end())
				return MGFTypeCaster::Cast<T>(it->second);
		}

		// 2. 락 밖에서 새 리소스 생성
		// name은 캐싱 키로만 사용하고, 나머지 인자들은 T::Create로 포워딩합니다.
		auto newResource = T::Create(name, std::forward<Args>(args)...);
		if (!newResource) return nullptr;

		// 3. 락 다시 잡고 한 번 더 확인 후 삽입
		{
			LockScope lock(m_mutex);
			auto it = m_namedCache.find(hash);
			if (it != m_namedCache.end()) return MGFTypeCaster::Cast<T>(it->second);
			m_namedCache[hash] = newResource;
		}

		// 4. 락 밖에서 RegisterSync 호출
		RegisterSync(newResource);

		return newResource;
	}

	template<typename T>
	std::shared_ptr<T> ResourceManager::Get(const String& name)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<NamedResource, T>,
			"T must inherit from NamedResource"
		);   

		StringHash hash(name);

		LockScope lock(m_mutex);
		auto it = m_namedCache.find(hash);
		if (it != m_namedCache.end()) return MGFTypeCaster::Cast<T>(it->second);
		return nullptr;
	}
}