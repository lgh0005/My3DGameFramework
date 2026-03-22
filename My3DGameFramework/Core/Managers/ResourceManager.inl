#pragma once

namespace MGF3D
{
	template<typename T>
	inline SharedPtr<T> ResourceManager::Get(const MGFName& assetName)
	{
		StringHash key = assetName.GetStringHash();

		// 1. 이미 로드된 캐시 확인
		auto value = m_resources.Find(key);
		if (value) return StaticSharedCast<T>(*value);

		// 2. 캐시에 없으면 m_assetRegistry 확인
		auto descriptorPtr = m_assetRegistry.Find(key);
		if (descriptorPtr)
		{
			auto resource = LoadResourceAsync((*descriptorPtr).Get());
			return StaticSharedCast<T>(resource);
		}

		MGF_LOG_WARN("ResourceManager: Resource not found in registry: {}", assetName.CStr());
		return nullptr;
	}
}