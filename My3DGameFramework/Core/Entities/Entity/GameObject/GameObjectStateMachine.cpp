#include "CorePch.h"
#include "GameObjectStateMachine.h"

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

		// TODO: 이 시점에 본체가 속한 Storage를 찾아 MarkDirty()를 호출해야 함
		// 예시:
		// Ptr<Component> comp = static_cast<Ptr<Component>>(m_owner);
		// if (auto* storage = Registry::GetStorage(comp->GetType())) 
		// {
		//     storage->MarkDirty();
		// }
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