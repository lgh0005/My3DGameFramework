#include "CorePch.h"
#include "GameObjectStateMachine.h"
#include "Entities/Entity/GameObject/GameObject.h"
#include "Entities/Entity/Component/Component.h"
#include "Managers/EntityManager.h"

namespace MGF3D
{
	GameObjectStateMachine::GameObjectStateMachine(Ptr<Object> owner) : Super(owner) 
	{
		m_state = GameObjectState::PendingAdd;
	}
	GameObjectStateMachine::~GameObjectStateMachine() = default;

	void GameObjectStateMachine::SetActive(bool active)
	{
		if (m_active == active) return;
		m_active = active;

		// 1. 본체(GameObject)를 가져옵니다.
		Ptr<GameObject> ownerGO = static_cast<Ptr<GameObject>>(m_owner);
		if (!ownerGO) return;

		// 2. 이 객체가 가진 모든 컴포넌트의 Storage를 찾아 MarkDirty를 호출
		for (auto& [type, comp] : ownerGO->GetComponents())
		{
			auto pStorage = MGF_ENTITY.GetStorageByType(type);
			if (pStorage) pStorage->MarkStorageDirty();
		}
	}

	bool GameObjectStateMachine::IsActive() const
	{
		return m_state == GameObjectState::Alive && m_active;
	}

	bool GameObjectStateMachine::IsDead() const
	{
		return m_state == GameObjectState::Dead;
	}
}