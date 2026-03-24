#pragma once

namespace MGF3D
{
	template<typename T>
	inline SharedPtr<T> ResourceManager::Get(const MGFName& resourceName)
	{
		StringHash key = resourceName.GetStringHash();
		auto resPtr = m_resources.Find(key);

		// 1. 이미 로드된 캐시 확인
		auto value = m_resources.Find(key);
		if (value) return StaticSharedCast<T>(*value);

		// 2. 타입 검사 및 캐스팅 후 반환
		if (resPtr)
		{
			//ResourcePtr res = *resPtr;
			//if (res->GetType().IsA(T::StaticType()))
			//	return StaticPointerCast<T>(res);
			//MGF_LOG_ERROR("ResourceManager: Type mismatch for resource '{}'", resourceName.CStr());
		}

		return nullptr;
	}
}