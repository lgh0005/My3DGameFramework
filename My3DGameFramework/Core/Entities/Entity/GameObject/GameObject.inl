#pragma once
#include "Managers/EntityManager.h"

namespace MGF3D
{
	template<typename T, typename ...Args>
	T* GameObject::AddComponent(Args && ...args)
	{
		return MGF_ENTITY.AddComponent<T>(this, std::forward<Args>(args)...);
	}

	template<typename T>
	T* GameObject::GetComponent() const
	{
		// 컨테이너에서 MGFType을 키로 검색
		auto compPtrAddr = m_components.Find(T::s_type.Get());
		if (compPtrAddr != nullptr) return static_cast<T*>(*compPtrAddr);
		return nullptr;
	}
}