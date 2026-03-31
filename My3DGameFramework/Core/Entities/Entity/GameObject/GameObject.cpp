#include "CorePch.h"
#include "GameObject.h"
#include "Entities/Entity/GameObject/GameObjectStateMachine.h"
#include "Entities/Entity/Component/Component.h"
#include "Managers/EntityManager.h"

namespace MGF3D
{
	GameObject::GameObject() = default;
	GameObject::~GameObject() = default;

	Ptr<GameObject> GameObject::Create()
	{
		// 1. 객체 할당
		auto go = UniquePtr<GameObject>(new GameObject());
		if (!go) return nullptr;

		// 2. 모든 권한을 Init에게 위임
		return go->Init(Move(go));
	}

	Ptr<GameObject> GameObject::Init(UniquePtr<GameObject>&& self)
	{
		// 1. 내부 상태 머신 생성
		m_stateMachine = MakeUnique<GameObjectStateMachine>(this);
		if (!m_stateMachine) return nullptr;
		
		// 2. 관리용 포인터 미리 확보
		Ptr<GameObject> ptr = self.Get();

		// 3. 엔진 매니저에게 소유권 이전 (정식 등록)
		MGF_ENTITY.AddGameObject(Move(self));

		// 4. 등록된 객체의 포인터 반환
		return ptr;
	}

	void GameObject::RegisterComponent(Ptr<Component> component)
	{
		if (component == nullptr) return;
		m_components.Insert(component->GetType(), component);
	}

	const GameObject::ComponentCache& GameObject::GetComponents() const
	{
		return m_components;
	}

	void GameObject::SetActive(bool active)
	{
		GetGameObjectStateMachine()->SetActive(active);
	}

	bool GameObject::IsActive() const
	{
		return GetGameObjectStateMachine()->IsActive();
	}

	Ptr<GameObjectStateMachine> GameObject::GetGameObjectStateMachine() const
	{
		return static_cast<Ptr<GameObjectStateMachine>>(m_stateMachine.Get());
	}
}

