#pragma once
#include "Entities/ECS/Storage.h"

namespace MGF3D
{
	// MGF_CLASS_PTR(GameObject)

	class EntityManager
	{
		MGF_DECLARE_SINGLE(EntityManager)

	private:
		EntityManager();
		~EntityManager();

	public:
		void OnPartition();

	public:
		template<typename T> Ptr<Storage<T>> GetStorage();

		//template<typename T, typename... Args>
		//Ptr<T> AddComponent(Ptr<GameObject> owner, Args&&... args);

		// TODO : Getcomponent도 추가 고려 필요

	private:
		SMap<usize, UniquePtr<IStorage>> m_storages;
	};
}

#include "Managers/EntityManager.inl"