#pragma once
#include "Entities/ECS/Storage.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)

	class EntityManager
	{
		MGF_DECLARE_SINGLE(EntityManager)

	private:
		EntityManager();
		~EntityManager();

	public:
		bool Init();
		void Update();
		void Shutdown();

	/*==========================================//
	//   Storage & Element Management           //
	//==========================================*/
	public:
		template<typename T> Ptr<Storage<T>> GetStorage();
		void AddGameObject(UniquePtr<GameObject>&& go);
		template<typename T, typename... Args>
		Ptr<T> AddComponent(Ptr<GameObject> owner, Args&&... args);

	/*=======================================//
	//   Object-Processing Methods           //
	//=======================================*/
	private:
		void PartitionObjects();
		void FixedUpdateObjects();
		void UpdateObjects();
		void LateUpdateObjects();

	/*=======================================//
	//   Pending Queue Processing Methods    //
	//=======================================*/
	public:
		void Clear();
		void ProcessPendingAddQueue();
		void ProcessPendingDestroyQueue();

	private:
		SMap<Ptr<const MGFType>, UniquePtr<IStorage>> m_storages;
		SVector<UniquePtr<GameObject>> m_gameObjects;
		SVector<UniquePtr<GameObject>> m_pendingAddQueue;
		SVector<Ptr<GameObject>> m_pendingDestroyQueue;
	};
}

#include "Managers/EntityManager.inl"