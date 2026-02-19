#include "EnginePch.h"
#include "GameObjectRegistry.h"
#include "Object/GameObject.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(GameObjectRegistry)

GameObjectRegistryUPtr GameObjectRegistry::Create()
{
	auto goMgr = GameObjectRegistryUPtr(new GameObjectRegistry());
	goMgr->Init();
	return std::move(goMgr);
}

void GameObjectRegistry::Init()
{
	m_gameObjects.reserve(4096);
	m_pendingCreateQueue.reserve(1024);
	m_pendingDestroyQueue.reserve(1024);

	m_gameObjectSlots.reserve(4096);
}

/*======================================//
//   default GameObject queue methods   //
//======================================*/
void GameObjectRegistry::AddGameObject(GameObjectUPtr go)
{
	if (!go) return;

	// 0. 중복 이름 검사
	std::string uniqueName = MakeUniqueObjectName(go->GetName());
	if (uniqueName != go->GetName()) go->SetName(uniqueName);

	// 1. 아이디 발급
	InstanceID id = RegisterGameObjectID(go.get());
	go->SetInstanceID(id);

	// 2. 게임 오브젝트 추가
	m_pendingCreateQueue.push_back(std::move(go));
}

void GameObjectRegistry::DestroyGameObject(GameObject* go)
{
	if (!go) return;

	// 이미 죽은 상태라면 중복 요청 무시
	if (go->IsDead()) return;

	// 플래그 설정 (GameObject 내부 상태 변경)
	go->SetDestroy();

	// 삭제 대기열에 등록 (Scene이 참조할 수 있도록)
	m_pendingDestroyQueue.push_back(go);

	// 자식들도 같이 순장 
	// TODO : 재귀를 flat하게
	Transform& transform = go->GetTransform();
	const auto& children = transform.GetChildren();
	for (Transform* child : children) DestroyGameObject(child->GetOwner());
}

std::string GameObjectRegistry::MakeUniqueObjectName(const std::string& baseName)
{
	// 1. 원본 이름으로 먼저 검색
	if (FindGameObjectByName(baseName) == nullptr) return baseName;

	// 2. 중복 발생 -> 접미사 붙이기 루프
	std::string newName;
	int32 index = 1;

	while (true)
	{
		newName = baseName + "_" + std::to_string(index);
		if (FindGameObjectByName(newName) == nullptr)
			return newName;
		index++;
	}
}

void GameObjectRegistry::PushToDestroyQueue(GameObject* go)
{
	if (!go) return;
	if (go->IsDead()) return;
	go->SetDestroy();
	m_pendingDestroyQueue.push_back(go);
}

void GameObjectRegistry::FlushCreateQueue()
{
	if (m_pendingCreateQueue.empty()) return;

	// 1. 현재 기존 데이터의 끝 위치 기억 및 벡터 크기 할당
	usize currentSize = m_gameObjects.size();
	usize addCount = m_pendingCreateQueue.size();
	usize requiredSize = currentSize + addCount;
	if (m_gameObjects.capacity() < requiredSize)
	{
		usize doubledCapacity = m_gameObjects.capacity() * 2;
		m_gameObjects.reserve(Utils::Max<usize>(requiredSize, doubledCapacity));
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

void GameObjectRegistry::FlushDestroyQueue()
{
	if (m_pendingDestroyQueue.empty()) return;

	// 1. SceneIndex를 이용한 Swap & Pop : O(k)
	for (GameObject* deadObj : m_pendingDestroyQueue)
	{
		// 1. ID 해제 (핸들 무효화) - 이제 외부에서 이 ID로 접근 불가
		UnregisterGameObjectID(deadObj->GetInstanceID());

		// 2. 실제 메모리 및 벡터에서 제거 (Swap & Pop)
		usize index = deadObj->GetSceneIndex();
		if (index >= m_gameObjects.size() || m_gameObjects[index].get() != deadObj)
			continue;

		// 2-1. Swap (맨 뒤 객체를 삭제된 자리로 가져오기)
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

const std::vector<GameObjectUPtr>& GameObjectRegistry::GetGameObjects() const
{
	return m_gameObjects;
}
const std::vector<GameObjectUPtr>& GameObjectRegistry::GetPendingCreateQueue() const
{
	return m_pendingCreateQueue;
}
const std::vector<GameObject*>& GameObjectRegistry::GetPendingDestroyQueue() const
{
	return m_pendingDestroyQueue;
}

/*================================//
//   GameObject finding methods   //
//================================*/
GameObject* GameObjectRegistry::GetGameObjectByID(InstanceID id)
{
	const uint32 index = GetIndexFromID(id);
	const uint32 gen = GetGenerationFromID(id);

	if (index >= m_gameObjectSlots.size()) return nullptr;

	const auto& slot = m_gameObjectSlots[index];
	if (slot.generation != gen) return nullptr;

	return slot.object;
}

GameObject* GameObjectRegistry::FindGameObjectByName(const std::string& name)
{
	uint32 targetHash = Utils::StrHash(name);

	// 2. Pending Queue 검색 (최신 우선)
	for (auto& go : m_pendingCreateQueue)
	{
		// [핵심] 문자열 비교 대신 정수(Hash) 비교 수행 (매우 빠름)
		if (go->GetNameHash() == targetHash)
		{
			// [안전장치] 해시 충돌 방지를 위해 실제 문자열 확인
			// 해시가 다르면 이 무거운 문자열 비교는 아예 실행되지 않음
			if (go->GetName() == name) return go.get();
		}
	}

	// 3. Active List 검색
	for (auto& go : m_gameObjects)
	{
		// [핵심] 정수 비교
		if (go->GetNameHash() == targetHash)
		{
			if (go->GetName() == name) return go.get();
		}
	}

	return nullptr;
}

InstanceID GameObjectRegistry::RegisterGameObjectID(GameObject* obj)
{
	uint32 index = 0;
	uint32 generation = 1;

	// 빈 슬롯 재사용
	if (m_freeSlotHead != 0xFFFFFFFF)
	{
		index = m_freeSlotHead;
		auto& slot = m_gameObjectSlots[index];
		m_freeSlotHead = slot.nextFree;

		slot.object = obj;
		slot.nextFree = 0xFFFFFFFF;
		generation = slot.generation;
	}
	// 새 슬롯 확장
	else
	{
		index = static_cast<uint32>(m_gameObjectSlots.size());
		GameObjectSlot newSlot;
		newSlot.object = obj;
		newSlot.generation = 1;
		newSlot.nextFree = 0xFFFFFFFF;
		m_gameObjectSlots.push_back(newSlot);
		generation = 1;
	}

	return MakeID(index, generation);
}

void GameObjectRegistry::UnregisterGameObjectID(InstanceID id)
{
	const uint32 index = GetIndexFromID(id);
	const uint32 gen = GetGenerationFromID(id);

	if (index >= m_gameObjectSlots.size()) return;

	auto& slot = m_gameObjectSlots[index];
	if (slot.generation != gen) return; // 이미 해제됨

	slot.object = nullptr;
	slot.generation++; // 세대 증가
	if (slot.generation == 0) slot.generation = 1;

	// Free List에 추가
	slot.nextFree = m_freeSlotHead;
	m_freeSlotHead = index;
}

uint32 GameObjectRegistry::GetIndexFromID(InstanceID id)
{
	return static_cast<uint32>(id & 0xFFFFFFFF);
}

uint32 GameObjectRegistry::GetGenerationFromID(InstanceID id)
{
	return static_cast<uint32>((id >> 32) & 0xFFFFFFFF);
}

InstanceID GameObjectRegistry::MakeID(uint32 index, uint32 gen)
{
	return (static_cast<InstanceID>(gen) << 32) | static_cast<InstanceID>(index);
}
