#pragma once
#include "Entities/ECS/Storage.h"
#include "Entities/Entity/Transform/TransformStorage.h"

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
		Ptr<IStorage> GetStorageByType(Ptr<const MGFType> type);
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

	/*===================================//
	//   Transform Management Methods    //
	//====================================*/
	public:
		// Transform 생성 및 제어 인터페이스
		usize CreateTransform(Ptr<GameObject> owner, const Position& pos, const Rotation& rot, const Scale& scale);
		void SetTransformParent(usize childIndex, usize parentIndex);

		// 외부에서 Transform 데이터에 접근할 때 사용
		Transform* GetTransform(usize index);
		void RemoveTransform(usize index);

	private:
		TransformStorage m_transformStorage;
		SMap<Ptr<const MGFType>, UniquePtr<IStorage>> m_storages;

		SVector<UniquePtr<GameObject>> m_gameObjects;
		SVector<UniquePtr<GameObject>> m_pendingAddQueue;
		SVector<Ptr<GameObject>> m_pendingDestroyQueue;
	};
}

#include "Managers/EntityManager.inl"