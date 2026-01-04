#include "EnginePch.h"
#include "GameObject.h"
#include "Scene/Scene.h"
#include "Components/Component.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(GameObject)

GameObjectUPtr GameObject::Create()
{
	auto gameObject = std::unique_ptr<GameObject>(new GameObject());
	if (!gameObject->Init()) return nullptr;
	return gameObject;
}

bool GameObject::Init()
{
	// 기본적으로 Transform을 소유
	auto transform = Transform::Create();
	m_transform = transform.get();
	if (!m_transform) return false;
	AddComponent(std::move(transform));
	return true;
}

/*==========================================//
//   game object life-cycle state methods   //
//==========================================*/
bool GameObject::IsActiveInHierarchy() const
{
	// 1. 나 자신이 꺼져있으면 false
	if (!m_active) return false;

	// 2. 부모가 있다면 부모에게 물어봄 (재귀)
	Transform* parentTransform = m_transform->GetParent();
	if (parentTransform) return parentTransform->GetOwner()->IsActiveInHierarchy();

	// 3. 부모가 없다면(루트) true
	return true;
}

void GameObject::Awake()
{
	// 이미 깨어났으면 무시
	if (m_state >= GameObjectState::Awake) return;

	// 모든 컴포넌트 Awake 호출 (비활성화된 컴포넌트도 Awake는 보통 호출)
	m_state = GameObjectState::Awake;
	for (auto& comp : m_components) comp->Awake();
}

void GameObject::Start()
{
	// 이미 시작했으면 무시
	if (m_state >= GameObjectState::Active) return;

	// 계층 구조상 활성화 상태여야 Start 진입 가능
	if (!IsActiveInHierarchy()) return;

	m_state = GameObjectState::Active;

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->Start();
	}
}

void GameObject::SetActive(bool active)
{
	if (m_active == active) return;

	// 상태 변경 전 계층 구조상 활성화 여부 확인
	bool wasActiveInHierarchy = IsActiveInHierarchy();

	m_active = active;

	// 상태 변경 후 계층 구조상 활성화 여부 확인
	bool nowActiveInHierarchy = IsActiveInHierarchy();

	if (wasActiveInHierarchy != nowActiveInHierarchy)
		SetActiveStateHierarchy(nowActiveInHierarchy);
}

void GameObject::FixedUpdate()
{
	// 계층 구조상 꺼져있으면 업데이트 스킵
	if (!IsActiveInHierarchy()) return;

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->FixedUpdate();
	}
}

void GameObject::Update()
{
	if (!IsActiveInHierarchy()) return;

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->Update();
	}
}

void GameObject::LateUpdate()
{
	if (!IsActiveInHierarchy()) return;

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->LateUpdate();
	}
}

void GameObject::SetDestroy()
{
	// 이미 죽었으면 무시
	if (m_state == GameObjectState::Dead) return;

	m_state = GameObjectState::Dead;
}

void GameObject::OnDestroy()
{
	// 실제 소멸 직전 호출
	for (auto& comp : m_components) comp->OnDestroy();
}

/*=====================//
//  component methods  //
//=====================*/
void GameObject::AddComponent(ComponentUPtr component)
{
	component->SetOwner(this);

	// 1. 먼저 벡터에 등록 (소유권 이동)
	m_components.push_back(std::move(component));

	// 2. 방금 넣은 컴포넌트 포인터 가져오기 (move 되었으므로 back()으로 접근)
	Component* newComp = m_components.back().get();

	// 3. 런타임 초기화 진행 (안전하게 GetComponent 가능)
	// TODO : 이 부분이 런타임 초기화 로직 부분임.
	// 따로 메서드로 작성할 필요가 있을 수 있음.
	if (m_state >= GameObjectState::Awake)
	{
		newComp->Awake();
	}

	if (m_state >= GameObjectState::Active && IsActiveInHierarchy())
	{
		if (newComp->IsEnabled())
		{
			newComp->OnEnable();
			newComp->Start();
		}
	}
}

/*=====================//
//  hierarchy methods  //
//=====================*/
void GameObject::SetParent(GameObject* parent)
{
	// 부모가 바뀌면 계층 활성화 상태가 바뀔 수 있음 (예: 꺼진 부모 -> 켜진 부모)
	bool wasActive = IsActiveInHierarchy();

	// parent가 nullptr이면 부모를 없애고(Root로 만든다는 의미) 부모 설정
	Transform* parentTransform = (parent != nullptr) ? &parent->GetTransform() : nullptr;
	m_transform->SetParent(parentTransform);

	bool nowActive = IsActiveInHierarchy();

	if (wasActive != nowActive) 
		SetActiveStateHierarchy(nowActive);
}

void GameObject::AddChild(GameObject* child)
{
	if (child == nullptr) return;
	child->SetParent(this);
}

void GameObject::SetActiveStateHierarchy(bool active)
{
	// 1. 내 컴포넌트들에게 이벤트 전파
	for (auto& comp : m_components)
	{
		// 컴포넌트 자체가 켜져있을 때만 OnEnable/Disable 반응
		if (comp->IsEnabled())
		{
			if (active) comp->OnEnable();
			else comp->OnDisable();
		}
	}

	// 2. 자식 게임 오브젝트들에게도 전파 (재귀)
	// 주의: 자식 자체가 SetActive(false)로 꺼져있다면, 
	// 부모가 켜지든 말든 걔는 계속 꺼져있어야 하므로 전파하면 안 됨.
	// 즉, 자식의 로컬 m_active가 true인 경우에만 계층 변화의 영향을 받음.
	const auto& children = m_transform->GetChildren();
	for (Transform* childTransform : children)
	{
		GameObject* childGO = childTransform->GetOwner();
		if (childGO && childGO->IsActive())
		{
			childGO->SetActiveStateHierarchy(active);
		}
	}
}
