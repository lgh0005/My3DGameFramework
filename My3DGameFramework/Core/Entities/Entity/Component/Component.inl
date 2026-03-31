#pragma once
#include "Managers/EntityManager.h"

namespace MGF3D
{
	template<typename T, typename ...Args>
	Ptr<T> Component::Create(Ptr<GameObject> owner, Args && ...args)
	{
		// 1. EntityManager를 통해 해당 타입의 최적화된 Storage에서 객체 생성(Emplace) 요청
		return MGF_ENTITY.AddComponent<T>(owner, std::forward<Args>(args)...);
	}
}