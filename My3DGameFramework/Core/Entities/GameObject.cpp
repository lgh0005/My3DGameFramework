#include "CorePch.h"
#include "GameObject.h"

namespace MGF3D
{
	GameObject::GameObject(ObjectIDHash id, StringHash nameHash) : Super(id), m_nameHash(nameHash) { }
	GameObject::~GameObject() = default;
	GameObject::GameObject(GameObject&&) noexcept = default;
	GameObject& GameObject::operator=(GameObject&&) noexcept = default;

    StringHash GameObject::GetNameHash() const
	{
		return m_nameHash;
	}

	void GameObject::SetActive(bool active)
	{
		EObjectLifecycle currentState = GetState();

		// 1. 이미 삭제 대기 중이거나 죽은 객체는 상태를 변경할 수 없음
		if (currentState == EObjectLifecycle::PendingKill || currentState == EObjectLifecycle::Dead)
			return;

		// 2. 목표 상태 결정
		EObjectLifecycle targetState = CommonUtils::Select(active, EObjectLifecycle::Active, EObjectLifecycle::Inactive);
		if (currentState == targetState) return;

		// 3. 상태 머신 업데이트
		SetState(targetState);

		// TODO: 이 객체의 상태가 변했으므로, 다음 프레임 Update 직전에 
		// 메모리 Defragmentation을 수행할 수 있도록 Registry의 Dirty Queue에 ID를 등록해야 함.
		// ex) m_registry->QueueStateChange(GetID());
	}

	bool GameObject::IsActiveSelf() const
	{
		return GetState() == EObjectLifecycle::Active;
	}
}