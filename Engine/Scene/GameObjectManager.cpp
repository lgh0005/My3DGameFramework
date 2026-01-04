#include "EnginePch.h"
#include "GameObjectManager.h"
#include "Scene/GameObject.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(GameObjectManager)

GameObjectManagerUPtr GameObjectManager::Create()
{
	return GameObjectManagerUPtr(new GameObjectManager());
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

GameObject* GameObjectManager::FindGameObject(const std::string& name)
{
	// 1. 방금 생성되어 대기 중인 녀석들 먼저 검색 (우선순위 높음)
	for (const auto& obj : m_pendingCreateQueue)
	{
		if (obj->GetName() == name) 
			return obj.get();
	}

	// 2. 이미 활동 중인 녀석들 검색
	for (const auto& ptr : m_gameObjects)
	{
		if (ptr->GetName() == name) 
			return ptr.get();
	}

	return nullptr;
}

void GameObjectManager::PushToDestroyQueue(GameObject* go)
{
	// 이미 죽을 예정인 녀석을 또 죽이면 안 됨 (중복 방지)
	if (std::find(m_pendingDestroyQueue.begin(), m_pendingDestroyQueue.end(), go)
		!= m_pendingDestroyQueue.end()) return;

	m_pendingDestroyQueue.push_back(go);
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

void GameObjectManager::FlushCreateQueue()
{
	if (m_pendingCreateQueue.empty()) return;

	// [최적화 1] 벡터 재할당 비용 최소화를 위한 예약
	m_gameObjects.reserve(m_gameObjects.size() + m_pendingCreateQueue.size());

	// [최적화 2] Move Iterator를 사용하여 소유권을 한 번에 이전
	m_gameObjects.insert
	(
		m_gameObjects.end(),
		std::make_move_iterator(m_pendingCreateQueue.begin()),
		std::make_move_iterator(m_pendingCreateQueue.end())
	);

	// 대기열 비우기 (이미 move 되었으므로 껍데기만 남음)
	m_pendingCreateQueue.clear();
}

void GameObjectManager::FlushDestroyQueue()
{
	if (m_pendingDestroyQueue.empty()) return;

	// [최적화 3] std::partition을 이용한 일괄 삭제
	// 죽은 객체들을 배열 끝으로 몰아넣고 한 번에 잘라냄 (Swap & Pop과 유사 효과)
	auto firstDead = std::partition
	(
		m_gameObjects.begin(),
		m_gameObjects.end(),
		[](const GameObjectUPtr& go) 
		{
			// 살아있는(!IsDead) 오브젝트가 앞쪽(true)으로 옴
			return !go->IsDead();
		}
	);

	// 뒤쪽에 모인 죽은 오브젝트들 일괄 삭제 (메모리 해제 발생)
	m_gameObjects.erase(firstDead, m_gameObjects.end());

	// 삭제 대기열 초기화
	m_pendingDestroyQueue.clear();
}