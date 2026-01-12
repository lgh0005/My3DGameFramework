#include "EnginePch.h"
#include "ObjectManager.h"

void ObjectManager::Init()
{
	m_gameObjectSlots.reserve(1024);
	m_userScriptComponentCache.reserve(1024);
}

void ObjectManager::Clear()
{
	// GameObject 삭제
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