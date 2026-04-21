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
	std::shared_ptr<T> ResourceManager::GetOrCreate(const String& name, Args&&... args)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<NamedResource, T>,
			"T must inherit from NamedResource"
		);

		StringHash hash(name);

		// 1. 캐시 히트 확인 (빠른 반환)
		{
			LockScope lock(m_mutex);
			auto it = m_namedCache.find(hash);
			if (it != m_namedCache.end())
				return MGFTypeCaster::Cast<T>(it->second);
		}

		// 2. 캐시 미스 시 새로 생성 (껍데기 생성)
		// name은 캐싱 키로만 사용하고, 나머지 인자들은 T::Create로 포워딩합니다.
		auto newResource = T::Create(name, std::forward<Args>(args)...);
		if (newResource)
		{
			LockScope lock(m_mutex);
			m_namedCache[hash] = newResource;
			RegisterSync(newResource);
		}

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