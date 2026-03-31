#include "CorePch.h"
#include "ComponentStateMachine.h"
#include "Entities/Entity/Component/Component.h"
#include "Entities/Entity/GameObject/GameObject.h"

namespace MGF3D
{
	ComponentStateMachine::ComponentStateMachine(Ptr<Object> owner)	: Super(owner) 
	{
		m_state = ComponentState::PendingAdd;
	}
	ComponentStateMachine::~ComponentStateMachine() = default;

	void ComponentStateMachine::SetEnabled(bool enabled)
	{
		if (m_enabled == enabled) return;
		m_enabled = enabled;
		
		// TODO: 이 시점에 본체가 속한 Storage를 찾아 MarkDirty()를 호출해야 함
		// 예시:
		// Ptr<Component> comp = static_cast<Ptr<Component>>(m_owner);
		// if (auto* storage = Registry::GetStorage(comp->GetType())) 
		// {
		//     storage->MarkDirty();
		// }
	}

	bool ComponentStateMachine::IsActive() const
	{
		// 1. 컴포넌트 생명주기가 Alive여야 함
		if (m_state != ComponentState::Alive) return false;

		// 2. 컴포넌트 자체가 Enabled여야 함
		if (!m_enabled) return false;

		// 3. 소유자(GameObject)가 Active 상태여야 함
		Ptr<Component> comp = static_cast<Ptr<Component>>(m_owner);
		Ptr<GameObject> ownerGO = comp->GetOwner();

		// 4. 소유자 상태 우선 반영
		if (ownerGO) return ownerGO->IsActive();

		return false;
	}

	bool ComponentStateMachine::IsDead() const
	{
		return m_state == ComponentState::Dead;
	}
}