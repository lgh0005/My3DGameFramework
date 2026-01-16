#include "EnginePch.h"
#include "ObjectManager.h"
#include "Object/Component.h"
#include "Object/GameObject.h"
#include "Components/Script.h"

#include "Scene/Scene.h"
#include "Scene/SceneRegistry.h"

DECLARE_DEFAULTS_IMPL(ObjectManager)

void ObjectManager::Init()
{
	m_gameObjectSlots.reserve(1024);
	m_userScriptComponentCache.reserve(1024);

	m_gameObjects.reserve(4096);
	m_pendingCreateQueue.reserve(1024);
	m_pendingDestroyQueue.reserve(1024);
}

void ObjectManager::Clear()
{
	// 게임 오브젝트들부터 전부 파괴
	m_gameObjects.clear();
	m_pendingCreateQueue.clear();
	m_pendingDestroyQueue.clear();

	// 슬롯 초기화
	m_goSlotfreeHead = 0xFFFFFFFF;
	m_gameObjectSlots.clear();

	// Component 삭제
	for (auto& vec : m_componentCache)			 vec.clear();
	for (auto& vec : m_userScriptComponentCache) vec.clear();

	LOG_INFO("ObjectManager Cleared.");
}

/*==========================================//
//   GameObject instance managing methods   //
//==========================================*/
InstanceID ObjectManager::RegisterGameObject(GameObject* obj)
{
	uint32 index = 0;
	uint32 generation = 1;

	// 1. 빈 슬롯이 있는 경우 (재사용)
	if (m_goSlotfreeHead != 0xFFFFFFFF)
	{
		index = m_goSlotfreeHead;
		GOSlot& slot = m_gameObjectSlots[index];
		m_goSlotfreeHead = slot.nextFree;

		// 슬롯 초기화
		slot.object = obj;
		slot.nextFree = 0xFFFFFFFF;
		generation = slot.generation;
	}

	// 2. 빈 슬롯이 없는 경우 (새로 확장)
	else
	{
		index = static_cast<uint32>(m_gameObjectSlots.size());

		GOSlot newSlot;
		newSlot.object = obj;
		newSlot.generation = 1;
		newSlot.nextFree = 0xFFFFFFFF;

		m_gameObjectSlots.push_back(newSlot);
		generation = 1;
	}

	return MakeGameObjectID(index, generation);
}

uint32 ObjectManager::GetGameObjectIndex(InstanceID id)
{
	// 하위 32비트 마스킹
	return static_cast<uint32>(id & 0xFFFFFFFF);
}

uint32 ObjectManager::GetGameObjectGeneration(InstanceID id)
{
	// 상위 32비트로 밀어서 마스킹
	return static_cast<uint32>((id >> 32) & 0xFFFFFFFF);
}

InstanceID ObjectManager::MakeGameObjectID(uint32 index, uint32 gen)
{
	// 64비트로 캐스팅 후 밀어야 데이터 손실 없음
	return (static_cast<InstanceID>(gen) << 32) | static_cast<InstanceID>(index);
}

void ObjectManager::UnregisterGameObject(InstanceID id)
{
	const uint32 index = GetGameObjectIndex(id);
	const uint32 gen = GetGameObjectGeneration(id);

	// 범위 체크
	if (index >= m_gameObjectSlots.size()) return;

	GOSlot& slot = m_gameObjectSlots[index];

	// 세대 체크 (이미 해제되었거나, 다른 객체가 살고 있으면 무시)
	if (slot.generation != gen) return;

	// 객체 포인터 제거
	slot.object = nullptr;

	// [핵심] 세대 증가 (이전 핸들 무효화)
	slot.generation++;
	if (slot.generation == 0) slot.generation = 1;

	// 빈칸 리스트(Stack 방식)의 맨 앞에 추가
	slot.nextFree = m_goSlotfreeHead;
	m_goSlotfreeHead = index;
}

GameObject* ObjectManager::GetGameObject(InstanceID id)
{
	const uint32 index = GetGameObjectIndex(id);
	const uint32 gen = GetGameObjectGeneration(id);
	if (index >= m_gameObjectSlots.size()) return nullptr;

	GOSlot& slot = m_gameObjectSlots[index];

	// 세대가 다르면(죽었거나 재사용됨) -> nullptr
	if (slot.generation != gen) return nullptr;

	return slot.object;
}

bool ObjectManager::IsGameObjectAlive(InstanceID id)
{
	return GetGameObject(id) != nullptr;
}

/*=========================================//
//   Component instance managing methods   //
//=========================================*/
void ObjectManager::RegisterComponent(Component* comp)
{
	if (!comp) return;

	ComponentType type = comp->GetComponentType();
	ComponentVector* components = nullptr;

	// 1. 저장될 벡터 찾기
	switch (type)
	{
		// 컴포넌트가 스크립트인 경우
		case ComponentType::Script:
		{
			Script* script = static_cast<Script*>(comp);
			ScriptID id = script->GetScriptID();

			// TODO : 메모리 재할당 로직 개선 필요
			if (m_userScriptComponentCache.size() <= id)
				m_userScriptComponentCache.resize(id + 1);

			components = &m_userScriptComponentCache[id];
			break;
		}

		// 그 외 일반적인 컴포넌트인 경우
		default:
		{
			usize index = static_cast<usize>(type);
			if (index < (usize)ComponentType::MAX)
				components = &m_componentCache[index];
			break;
		}
	}

	// 2. 컴포넌트의 RegistryIndex 갱신
	if (components)
	{
		components->push_back(comp);
		comp->SetRegistryIndex(components->size() - 1);
	}
}

void ObjectManager::UnregisterComponent(Component* comp)
{
	if (!comp) return;

	usize indexToRemove = comp->GetRegistryIndex();
	if (indexToRemove == INVALID_INDEX) return;

	ComponentType type = comp->GetComponentType();
	ComponentVector* components = nullptr;

	// 1. 저장될 벡터 찾기
	switch (type)
	{
		// 컴포넌트가 스크립트인 경우
		case ComponentType::Script:
		{
			Script* script = static_cast<Script*>(comp);
			ScriptID id = script->GetScriptID();

			if (id < m_userScriptComponentCache.size())
				components = &m_userScriptComponentCache[id];
			break;
		}

		// 그 외 일반적인 컴포넌트인 경우
		default:
		{
			usize index = static_cast<usize>(type);
			if (index < (usize)ComponentType::MAX)
				components = &m_componentCache[index];
			break;
		}
	}

	// 2. 등록 컴포넌트를 Swap & pop idiom을 이용해서 삭제
	if (components)
	{
		ComponentVector& vec = *components;
		if (vec.empty()) return;

		usize lastIndex = vec.size() - 1;
		if (indexToRemove <= lastIndex)
		{
			if (indexToRemove != lastIndex)
			{
				Component* lastComp = vec[lastIndex];
				vec[indexToRemove] = lastComp;
				lastComp->SetRegistryIndex(indexToRemove);
			}
			vec.pop_back();
			comp->SetRegistryIndex(INVALID_INDEX);
		}
	}
}

/*===================================//
//   runtime GameObject management   //
//===================================*/
void ObjectManager::AddGameObject(GameObjectUPtr go)
{
	if (!go) return;
	m_pendingCreateQueue.push_back(std::move(go));
}

void ObjectManager::DestroyGameObject(GameObject* go)
{
	if (!go) return;

	// 이미 죽은 상태라면 중복 요청 무시
	// (IsValid를 쓰면 더 안전하지만, 내부 로직이므로 IsDead 체크로도 충분)
	if (go->IsDead()) return;

	// 플래그 설정
	go->SetDestroy();

	// 삭제 대기열 등록
	m_pendingDestroyQueue.push_back(go);

	// 자식들도 같이 순장 (재귀 호출)
	Transform& transform = go->GetTransform();
	const auto& children = transform.GetChildren();
	for (Transform* child : children) DestroyGameObject(child->GetOwner());
}

void ObjectManager::FlushCreateQueue()
{
	if (m_pendingCreateQueue.empty()) return;

	// 1. 사이즈 계산
	usize currentSize = m_gameObjects.size();
	usize addCount = m_pendingCreateQueue.size();
	usize requiredSize = currentSize + addCount;
	if (m_gameObjects.capacity() < requiredSize)
	{
		usize doubledCapacity = m_gameObjects.capacity() * 2;
		m_gameObjects.reserve(Utils::Max<usize>(requiredSize, doubledCapacity));
	}

	// 2. [이동] 소유권이 m_gameObjects로 넘어갑니다.
	// 주의: 이 시점 이후 m_pendingCreateQueue 내부의 unique_ptr들은 nullptr가 됩니다.
	m_gameObjects.insert
	(
		m_gameObjects.end(),
		std::make_move_iterator(m_pendingCreateQueue.begin()),
		std::make_move_iterator(m_pendingCreateQueue.end())
	);

	// 3. 인덱스 갱신 및 ID 등록
	for (usize i = 0; i < addCount; ++i)
	{
		GameObject* addedObj = m_gameObjects[currentSize + i].get();
		addedObj->SetSceneIndex(currentSize + i);
	}

	// 4. [비우기] 껍데기만 남은 대기열을 확실하게 초기화합니다.
	m_pendingCreateQueue.clear();
}

void ObjectManager::FlushDestroyQueue()
{
	if (m_pendingDestroyQueue.empty()) return;

	// 1. SceneIndex를 이용한 Swap & Pop : O(k)
	for (GameObject* deadObj : m_pendingDestroyQueue)
	{
		// [삭제] m_liveObjectSet.erase(deadObj);

		// [추가/중요] 슬롯맵(ID 시스템)에서 이 객체를 말소시켜야 함!
		// 이제 이 ID를 가진 녀석은 "죽은 세대"가 되어 IsGameObjectAlive(id)가 false를 반환함.
		UnregisterGameObject(deadObj->GetInstanceID());

		// 1. SceneIndex를 이용한 Swap & Pop 로직
		usize index = deadObj->GetSceneIndex();

		if (index >= m_gameObjects.size() || m_gameObjects[index].get() != deadObj)
			continue;

		// 1-1. Swap
		usize lastIndex = m_gameObjects.size() - 1;
		if (index != lastIndex)
		{
			std::swap(m_gameObjects[index], m_gameObjects[lastIndex]);
			m_gameObjects[index]->SetSceneIndex(index);
		}

		// 1-2. 메모리 해제 (소멸자 호출)
		m_gameObjects.pop_back();
	}

	m_pendingDestroyQueue.clear();
}

void ObjectManager::ProcessPendingCreates(bool isSceneAwake, bool isSceneRunning)
{
	if (m_pendingCreateQueue.empty()) return;

	// 1. [순회] 이동하기 전이므로 unique_ptr들이 살아있습니다.
	for (const auto& go : m_pendingCreateQueue)
	{
		// 1-1. 컴포넌트 등록 (Scene에 있던 로직 이동)
		for (const auto& comp : go->GetComponents())
		{
			RegisterComponent(comp.get());
		}

		// 1-2. Lifecycle Catch-up (Scene 상태에 맞춰 따라잡기)
		if (isSceneAwake) go->Awake();
		if (isSceneRunning && go->IsActive()) go->Start();
	}

	// 2. [이동 및 비우기] 처리가 다 끝났으니 안전하게 옮깁니다.
	FlushCreateQueue();
}

void ObjectManager::ProcessPendingDestroys()
{
	if (m_pendingDestroyQueue.empty()) return;

	for (GameObject* deadObj : m_pendingDestroyQueue)
	{
		// 1. 최후의 유언
		deadObj->OnDestroy();

		// 2. 컴포넌트 말소
		for (const auto& comp : deadObj->GetComponents())
		{
			UnregisterComponent(comp.get());
		}

		// 3. 부모 연결 끊기
		// Transform은 Component이므로 헤더 include만 되어 있으면 접근 가능
		Transform& myTransform = deadObj->GetTransform();
		Transform* parentTransform = myTransform.GetParent();
		if (parentTransform)
		{
			GameObject* parentGO = parentTransform->GetOwner();
			if (parentGO && !parentGO->IsDead()) myTransform.SetParent(nullptr);
		}
	}

	// 4. 메모리 해제
	FlushDestroyQueue();
}

const std::vector<GameObjectUPtr>& ObjectManager::GetGameObjects() const
{
	return m_gameObjects;
}

const std::vector<GameObjectUPtr>& ObjectManager::GetPendingCreateQueue() const
{
	return m_pendingCreateQueue;
}

const std::vector<GameObject*>& ObjectManager::GetPendingDestroyQueue() const
{
	return m_pendingDestroyQueue;
}