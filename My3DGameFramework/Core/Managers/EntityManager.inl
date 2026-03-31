#pragma once
#include "Entities/Entity/Component/Component.h"
#include "Entities/Entity/GameObject/GameObject.h" 
#include "Entities/ECS/Storage.h"

namespace MGF3D
{
	template<typename T>
	inline Ptr<Storage<T>> EntityManager::GetStorage()
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Component, T>,
			"EntityManager::GetStorage<T>: T must inherit from MGF3D::Component!"
		);

		// 1. T 타입의 정적 타입 정보(MGFType) 주소를 키로 사용
		const MGFType* type = T::s_type.Get();
		if (!m_storages.Contains(type))
			m_storages.Insert(type, MakeUnique<Storage<T>>());

		return static_cast<Storage<T>*>(m_storages[type].Get());
	}

	template<typename T, typename ...Args>
	inline Ptr<T> EntityManager::AddComponent(Ptr<GameObject> owner, Args && ...args)
	{
		// 1. 해당 타입의 스토리지를 가져오거나 생성
		auto storage = GetStorage<T>();

		// 2. Storage 내부의 Chunk(Slab)에서 메모리를 할당받아 객체 생성
		Ptr<T> comp = storage->Emplace(std::forward<Args>(args)...);

		// 3. 소유자 설정 및 GameObject 내부 리스트에 추가 (캐스팅 활용)
		// GameObject에 등록
		comp->SetOwner(owner);
		owner->RegisterComponent(comp);

		return comp;
	}
}