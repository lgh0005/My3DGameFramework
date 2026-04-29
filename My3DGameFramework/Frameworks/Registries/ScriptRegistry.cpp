#include "FrameworkPch.h"
#include "ScriptRegistry.h"
#include "Components/Script/Script.h"

namespace MGF3D
{
	ScriptRegistry::ScriptRegistry() = default;
	ScriptRegistry::~ScriptRegistry() = default;

	void ScriptRegistry::Init()
	{
		m_scripts.reserve(256);
		m_pendingAdds.reserve(64);
		m_pendingKills.reserve(64);
	}

	void ScriptRegistry::Update()
	{
		FlushPendingKills(); // 1. 먼저 죽여서 m_freeSlots를 확보
		FlushPendingAdds();  // 2. 확보된 빈자리에 새 스크립트들을 쏙쏙 집어넣음
	}

	void ScriptRegistry::Shutdown()
	{
		Clear();
	}

	void ScriptRegistry::Clear()
	{
		m_pendingAdds.clear();
		m_pendingKills.clear();
		m_freeSlots.clear();
		m_ownerToIndices.clear();
		m_scripts.clear();
	}

	void ScriptRegistry::AddScript(ScriptUPtr script)
	{
		if (script == nullptr) return;
		script->SetState(EObjectLifecycle::PendingAdd);
		m_pendingAdds.push_back(std::move(script));
	}

	Script* ScriptRegistry::GetScript(ObjectIDHash ownerID)
	{
		if (!ownerID.IsValid()) return nullptr;

		// O(1) 해시맵 탐색
		auto it = m_ownerToIndices.find(ownerID);
		if (it != m_ownerToIndices.end() && !it->second.empty())
		{
			// 해당 엔티티가 가진 첫 번째 스크립트를 반환
			usize index = it->second[0];
			if (m_scripts[index]) return m_scripts[index].get();
		}

		return nullptr;
	}

	void ScriptRegistry::RemoveScript(ObjectIDHash ownerID)
	{
		if (!ownerID.IsValid()) return;
		m_pendingKills.push_back(ownerID);
	}

	const Vector<ScriptUPtr>& ScriptRegistry::GetScripts() const
	{
		return m_scripts;
	}

	void ScriptRegistry::FlushPendingAdds()
	{
		if (m_pendingAdds.empty()) return;

		for (auto& script : m_pendingAdds)
		{
			script->Awake();
			script->SetState(EObjectLifecycle::PendingStart);

			ObjectIDHash ownerID = script->GetOwnerID();
			usize targetIndex = 0;

			// 1. 빈자리가 있으면 재사용, 없으면 끝에 추가
			if (!m_freeSlots.empty())
			{
				targetIndex = m_freeSlots.back();
				m_freeSlots.pop_back();
				m_scripts[targetIndex] = std::move(script);
			}
			else
			{
				targetIndex = m_scripts.size();
				m_scripts.push_back(std::move(script));
			}

			// 2. 해시맵 캐시에 인덱스 등록 (O(1))
			m_ownerToIndices[ownerID].push_back(targetIndex);
		}
		m_pendingAdds.clear();
	}

	void ScriptRegistry::FlushPendingKills()
	{
		if (m_pendingKills.empty()) return;

		for (ObjectIDHash ownerID : m_pendingKills)
		{
			// 3. 해시맵에서 해당 엔티티의 스크립트 인덱스들을 즉시 찾음 (O(1))
			auto it = m_ownerToIndices.find(ownerID);
			if (it != m_ownerToIndices.end())
			{
				const Vector<usize>& indices = it->second;
				for (usize index : indices)
				{
					if (m_scripts[index])
					{
						// 4. 인덱스로 핀포인트 저격 삭제 (O(1))
						m_scripts[index]->SetState(EObjectLifecycle::Dead);
						m_scripts[index].reset();
						m_freeSlots.push_back(index);
					}
				}

				// 5. 캐시에서 엔티티 기록 삭제
				m_ownerToIndices.erase(it);
			}
		}
		m_pendingKills.clear();
	}
}