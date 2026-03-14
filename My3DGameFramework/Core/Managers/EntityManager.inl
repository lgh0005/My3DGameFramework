#pragma once
// #include "Entities/Entity/Component.h"

namespace MGF3D
{
	template<typename T>
	inline Ptr<Storage<T>> EntityManager::GetStorage()
	{
		// TODO: 나중에 구상하신 커스텀 RTTI 해시 시스템으로 교체하세요!
			// 지금은 임시로 컴파일러의 typeid 해시 등을 활용할 수 있습니다.
		//const size_t typeID = GetTypeID<T>();

		//if (!m_storages.Contains(typeID))
		//{
		//	// Storage 생성 및 UniquePtr로 소유권 확보
		//	m_storages[typeID] = MakeUnique<Storage<T>>();
		//}

		//// 부모 인터페이스(IStorage)를 실제 타입(Storage<T>)으로 캐스팅하여 반환
		//return static_cast<Storage<T>*>(m_storages[typeID].Get());

		return Ptr<Storage<T>>();
	}

	//template<typename T, typename ...Args>
	//inline Ptr<T> EntityManager::AddComponent(Ptr<GameObject> owner, Args && ...args)
	//{
	//	auto* storage = GetStorage<T>();
	//	Ptr<T> comp = storage->Emplace(std::forward<Args>(args)...);
	//	comp->SetOwner(owner);
	//	return comp;
	//}

}