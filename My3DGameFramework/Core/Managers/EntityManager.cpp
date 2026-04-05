#include "CorePch.h"
#include "EntityManager.h"
#include "Entities/ECS/Storage.h"
#include "Entities/Entity/GameObject/GameObject.h"
#include "Entities/Entity/Component/Component.h"
#include "Entities/Entity/Component/ComponentStateMachine.h"
#include "Entities/Entity/GameObject/GameObjectStateMachine.h"

namespace MGF3D
{
	EntityManager::EntityManager() = default;
	EntityManager::~EntityManager() = default;

	bool EntityManager::Init()
	{
		m_gameObjects.Reserve(1024);
		m_pendingAddQueue.Reserve(128);
		m_pendingDestroyQueue.Reserve(128);
		return true;
	}

	void EntityManager::Update()
	{
		// 1. 신규 객체 정식 등록 (Pending -> Alive)
		// 이 과정에서 새로운 컴포넌트들이 추가되면서 Storage가 Dirty 상태가 됩니다.
		ProcessPendingAddQueue();

		// 2. 대열 재정렬 (Defragment)
		// Pending에서 들어온 신입들과, 지난 프레임에 SetEnabled(false) 된 녀석들을 
		// 뒤로 밀어내고 활성 객체만 앞으로 꽉 채웁니다.
		PartitionObjects();

		// 3. 이제 '연속된 활성 구간'만 골라서 업데이트 수행
		FixedUpdateObjects();
		UpdateObjects();
		LateUpdateObjects();

		// 4. 프레임 끝에서 삭제 처리 (메모리 해제 등)
		ProcessPendingDestroyQueue();
	}

	void EntityManager::Shutdown()
	{
		Clear();
		MGF_LOG_INFO("EntityManager: All Objects have been cleared.");
	}

	/*==========================================//
	//   Storage & Element Management           //
	//==========================================*/
	Ptr<IStorage> EntityManager::GetStorageByType(Ptr<const MGFType> type)
	{
		auto it = m_storages.Find(type);
		if (it != nullptr) return it->Get();
		return nullptr;
	}

	void EntityManager::AddGameObject(UniquePtr<GameObject>&& go)
	{
		if (go) m_pendingAddQueue.PushBack(Move(go));
	}

	/*=======================================//
	//   Object-Processing Methods           //
	//=======================================*/
	void EntityManager::PartitionObjects()
	{
		for (auto& [type, storage] : m_storages) storage->Defragment();
	}

	void EntityManager::FixedUpdateObjects()
	{
		for (auto& [type, storage] : m_storages) storage->FixedUpdate();
	}

	void EntityManager::UpdateObjects()
	{
		for (auto& [type, storage] : m_storages) storage->Update();
	}

	void EntityManager::LateUpdateObjects()
	{
		for (auto& [type, storage] : m_storages) storage->LateUpdate();
	}

	void EntityManager::Clear()
	{
		// 1. 대기열 먼저 청소
		// 태어나지도 못한 녀석들과 죽기를 기다리던 포인터들을 먼저 정리합니다.
		m_pendingAddQueue.Clear();
		m_pendingDestroyQueue.Clear();

		// 2. 모든 GameObject 소멸
		// 메인 컨테이너를 비우면 UniquePtr이 파괴되면서 GameObject와 그 내부 데이터가 해제됩니다.
		// 이때 GameObject가 들고 있던 컴포넌트 Ptr들은 단순 포인터이므로 안전하게 사라집니다.
		m_gameObjects.Clear();

		// 3. 모든 Storage 초기화 및 파괴
		// 각 Storage가 들고 있는 SVector<UniquePtr<Chunk<T>>>가 비워지면서 
		// 16KB 뭉텅이 메모리(Chunk)들이 전부 SlabMemoryPool로 반환됩니다.
		m_storages.Clear();
	}

	/*=======================================//
	//   Pending Queue Processing Methods    //
	//=======================================*/
	void EntityManager::ProcessPendingAddQueue()
	{
		if (m_pendingAddQueue.Empty()) return;

		for (auto& go : m_pendingAddQueue)
		{
			// 1. 상태 전환: PendingAdd -> Alive
			auto gsm = go->GetGameObjectStateMachine();
			gsm->SetActive(true);

			// 2. 초기 생명주기 호출
			// 여기에서는 게임 오브젝트의 Awake랑 Start를 호출
			for (auto& [compType, compPtr] : go->GetComponents())
			{
				compPtr->Awake();
				compPtr->Start();
			}

			// 3. 소유권을 메인 컨테이너로 완전히 이동
			m_gameObjects.PushBack(Move(go));
		}

		m_pendingAddQueue.Clear();
	}

	void EntityManager::ProcessPendingDestroyQueue()
	{
		if (m_pendingDestroyQueue.Empty()) return;

		for (auto goPtr : m_pendingDestroyQueue)
		{
			if (goPtr == nullptr) continue;

			// 1. 해당 GameObject가 가진 모든 컴포넌트를 각 Storage에서 제거
			for (auto& [compType, compPtr] : goPtr->GetComponents())
			{
				auto pStorage = m_storages.Find(compType);
				if (pStorage != nullptr)
					(*pStorage)->Remove(compPtr->GetStorageIndex());
			}

			// 2. 메인 컨테이너에서 해당 GameObject 삭제
			usize targetIdx = goPtr->GetStorageIndex();
			usize lastIdx = m_gameObjects.Count() - 1;

			if (targetIdx != lastIdx)
				m_gameObjects.Back()->SetStorageIndex(targetIdx);

			m_gameObjects.RemoveSwap(targetIdx);
		}

		m_pendingDestroyQueue.Clear();
	}
}