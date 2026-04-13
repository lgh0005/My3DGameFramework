#include "CorePch.h"
#include "GameObjectRegistry.h"
#include "Managers/NameManager.h"

namespace MGF3D
{
	GameObjectRegistry::GameObjectRegistry() = default;
	GameObjectRegistry::~GameObjectRegistry() = default;

	void GameObjectRegistry::Init()
	{
		m_freeIndices.reserve(1024);
		m_generations.resize(1024, 0);
		m_pendingAdds.reserve(128);
		m_pendingKills.reserve(128);
		m_dirtyStateQueue.reserve(128);
	}

	void GameObjectRegistry::Update()
	{
		// 1. Defragmentation (상태 변경 동기화 및 스왑)
		DefragmentGameObject();

		// 2. 생성/삭제 대기열 처리
		FlushPendingAdds();
		FlushPendingKills();
	}

	void GameObjectRegistry::Shutdown()
	{
		Clear();
	}

	void GameObjectRegistry::Clear()
	{
		m_pendingAdds.clear();
		m_pendingKills.clear();
		m_dirtyStateQueue.clear();
		m_gameObjects.Clear();
		m_nextIndex = 1;
		m_freeIndices.clear();
		m_generations.assign(m_generations.size(), 0);
	}

	ObjectIDHash GameObjectRegistry::AddGameObject(const String& name)
	{
		StringHash uniqueNameHash = MGF_NAME.RegisterUnique(name);
		ObjectIDHash newID = GenerateNewID();
		m_pendingAdds.emplace_back(newID, uniqueNameHash);
		return newID;
	}

	GameObject* GameObjectRegistry::GetGameObject(const String& name)
	{
		StringHash nameHash(name);
		auto it = m_nameLookupTable.find(nameHash);
		if (it == m_nameLookupTable.end()) return nullptr;
		return m_gameObjects.Get(it->second.GetIndex());
	}

	void GameObjectRegistry::DestroyGameObject(const String& name)
	{
		StringHash nameHash(name);
		auto it = m_nameLookupTable.find(nameHash);
		if (it != m_nameLookupTable.end())
			m_pendingKills.push_back(it->second);
	}

	GameObject* GameObjectRegistry::GetGameObject(ObjectIDHash id)
	{
		uint32 index = id.GetIndex();
		if (index >= m_generations.size()) return nullptr;
		if (m_generations[index] != id.GetGeneration()) return nullptr;
		return m_gameObjects.Get(index);
	}

	void GameObjectRegistry::DestroyGameObject(ObjectIDHash id)
	{
		uint32 index = id.GetIndex();
		if (index >= m_generations.size()) return;

		// 세대가 일치하는 유효한 객체일 때만 파괴 대기열에 추가
		if (m_generations[index] == id.GetGeneration())
			m_pendingKills.push_back(id);
	}

	void GameObjectRegistry::FlushPendingAdds()
	{
		if (m_pendingAdds.empty()) return;

		for (auto& go : m_pendingAdds)
		{
			int32 id = go.GetID().GetIndex();

			{
				// NameManager를 통해 해시값으로 원본 문자열을 복원하여 출력합니다.
				String objName = MGF_NAME.GetString(go.GetNameHash());
				MGF_LOG_ERROR("FlushPendingAdds - Index: {0}, Name: {1}", id, objName);
			}

			if (m_gameObjects.Has(id)) continue;

			m_nameLookupTable[go.GetNameHash()] = go.GetID();
			go.SetState(EObjectLifecycle::Active);
			m_gameObjects.Emplace(id, std::move(go));
			m_gameObjects.MoveToFrontGroup(id);
		}

		m_pendingAdds.clear();
	}

	void GameObjectRegistry::FlushPendingKills()
	{
		if (m_pendingKills.empty()) return;

		for (ObjectIDHash id : m_pendingKills)
		{
			GameObject* go = m_gameObjects.Get(id.GetIndex());
			if (go) m_nameLookupTable.erase(go->GetNameHash());
			m_gameObjects.Remove(id.GetIndex());
			ReleaseID(id);
		}

		m_pendingKills.clear();
	}

	ObjectIDHash GameObjectRegistry::GenerateNewID()
	{
		uint32 index;

		if (!m_freeIndices.empty())
		{
			index = m_freeIndices.back();
			m_freeIndices.pop_back();
		}
		else
		{
			index = m_nextIndex++;
			if (index >= static_cast<uint32>(m_generations.size()))
				m_generations.resize(index * 2, 0);
		}

		return ObjectIDHash(index, m_generations[index]);
	}

	void GameObjectRegistry::ReleaseID(ObjectIDHash id)
	{
		uint32 index = id.GetIndex();
		m_generations[index]++;
		m_freeIndices.push_back(index);
	}

	void GameObjectRegistry::DefragmentGameObject()
	{
		if (!m_dirtyStateQueue.empty())
		{
			for (ObjectIDHash idHash : m_dirtyStateQueue)
			{
				int32 id = idHash.GetIndex();
				GameObject* obj = m_gameObjects.Get(id);
				if (!obj) continue;

				if (obj->IsActiveSelf()) m_gameObjects.MoveToFrontGroup(id);
				else m_gameObjects.MoveToBackGroup(id);
			}
			m_dirtyStateQueue.clear();
		}
	}
}