#include "EnginePch.h"
#include "GameObjectManager.h"
#include "Object/GameObject.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(GameObjectManager)

GameObjectManagerUPtr GameObjectManager::Create()
{
	auto goMgr = GameObjectManagerUPtr(new GameObjectManager());
	goMgr->Init();
	return std::move(goMgr);
}

void GameObjectManager::Init()
{
	m_gameObjects.reserve(4096);
	m_pendingCreateQueue.reserve(1024);
	m_pendingDestroyQueue.reserve(1024);
}

void GameObjectManager::AddGameObject(GameObjectUPtr go)
{
	if (!go) return;
	m_pendingCreateQueue.push_back(std::move(go));
}

void GameObjectManager::DestroyGameObject(GameObject* go)
{
	if (!go) return;

	// 이미 죽은 상태라면 중복 요청 무시
	if (go->IsDead()) return;

	// 플래그 설정 (GameObject 내부 상태 변경)
	go->SetDestroy();

	// 삭제 대기열에 등록 (Scene이 참조할 수 있도록)
	m_pendingDestroyQueue.push_back(go);

	// 자식들도 같이 순장
	Transform& transform = go->GetTransform();
	const auto& children = transform.GetChildren();
	for (Transform* child : children) DestroyGameObject(child->GetOwner());
}

void GameObjectManager::PushToDestroyQueue(GameObject* go)
{
	if (!go) return;
	if (go->IsDead()) return;
	go->SetDestroy();
	m_pendingDestroyQueue.push_back(go);
}

void GameObjectManager::FlushCreateQueue()
{
	if (m_pendingCreateQueue.empty()) return;

	// 1. 현재 기존 데이터의 끝 위치 기억 및 벡터 크기 할당
	usize currentSize = m_gameObjects.size();
	usize addCount = m_pendingCreateQueue.size();
	usize requiredSize = currentSize + addCount;
	if (m_gameObjects.capacity() < requiredSize)
	{
		// TODO : Utils에 대해서 Max부분은 GLMVec3Max, GLMVec3Min으로 수정하고
		// 두 수 비교에 대해서는 Min, Max로 만들어야 할 필요가 있을 듯.
		usize doubledCapacity = m_gameObjects.capacity() * 2;
		m_gameObjects.reserve((requiredSize > doubledCapacity ? requiredSize : doubledCapacity));
	}

	// 2. 소유권 일괄 이동
	// INFO : 평균 O(k), 최악의 경우 벡터 메모리 재할당으로 인한 O(n + k)
	m_gameObjects.insert
	(
		m_gameObjects.end(),
		std::make_move_iterator(m_pendingCreateQueue.begin()),
		std::make_move_iterator(m_pendingCreateQueue.end())
	);

	// 3. 게임 오브젝트 메모리 위치를 위한 인덱스 갱신 O(k)
	for (usize i = 0; i < addCount; ++i)
		m_gameObjects[currentSize + i]->SetSceneIndex(currentSize + i);

	// 4. 대기열 비우기 (이미 move 되었으므로 껍데기만 남음)
	m_pendingCreateQueue.clear();
}

void GameObjectManager::FlushDestroyQueue()
{
	if (m_pendingDestroyQueue.empty()) return;

	// 1. SceneIndex를 이용한 Swap & Pop : O(k)
	for (GameObject* deadObj : m_pendingDestroyQueue)
	{
		usize index = deadObj->GetSceneIndex();

		if (index >= m_gameObjects.size() || m_gameObjects[index].get() != deadObj)
			continue;

		// 1-1. Swap (맨 뒤 객체를 삭제된 자리로 가져오기)
		usize lastIndex = m_gameObjects.size() - 1;
		if (index != lastIndex)
		{
			std::swap(m_gameObjects[index], m_gameObjects[lastIndex]);
			m_gameObjects[index]->SetSceneIndex(index);
		}

		// 1-2. 맨 뒤로 이동된 죽은 객체를 제거
		m_gameObjects.pop_back();
	}

	// 2. 삭제 대기열 초기화
	m_pendingDestroyQueue.clear();
}

const std::vector<GameObjectUPtr>& GameObjectManager::GetGameObjects() const
{
	return m_gameObjects;
}
const std::vector<GameObjectUPtr>& GameObjectManager::GetPendingCreateQueue() const
{
	return m_pendingCreateQueue;
}
const std::vector<GameObject*>& GameObjectManager::GetPendingDestroyQueue() const
{
	return m_pendingDestroyQueue;
}