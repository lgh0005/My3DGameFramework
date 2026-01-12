#include "EnginePch.h"
#include "GameObject.h"
#include "Scene/Scene.h"
#include "Object/Component.h"
#include "Components/Transform.h"
#include "Components/Script.h"

GameObject::GameObject() = default;
GameObject::~GameObject()
{
	// 1. 내가 가진 모든 컴포넌트를 ObjectManager 캐시에서 제거
	for (auto& comp : m_components)
	{
		if (comp) OBJECT.UnregisterComponent(comp.get());
	}

	// 2. this(GameObject)을 ObjectManager 관리 대장에서 말소
	if (OBJECT.IsGameObjectAlive(m_instanceID))
		OBJECT.UnregisterGameObject(m_instanceID);
}

GameObjectUPtr GameObject::Create()
{
	auto gameObject = std::unique_ptr<GameObject>(new GameObject());
	if (!gameObject->Init()) return nullptr;
	return gameObject;
}

bool GameObject::Init()
{
	// ObjectManager에 자신을 등록
	m_componentCache.fill(nullptr);
	m_instanceID = OBJECT.RegisterGameObject(this);

	// 기본적으로 Transform을 소유
	auto transform = Transform::Create();
	AddComponent(std::move(transform));
	return true;
}

Transform& GameObject::GetTransform() const
{
	return *static_cast<Transform*>(m_componentCache[(usize)ComponentType::Transform]);
}

/*=============================================//
//   GameObject life-cycle methods by engine   //
//=============================================*/
void GameObject::Awake()
{
	// 1. 이미 Awake를 거쳤다면 return
	if (IsAwake()) return;

	// 2. 상태를 Awake로 전환
	SetGameObjectState(GameObjectState::Awake);

	// 모든 컴포넌트 Awake 호출
	for (auto& comp : m_components) comp->Awake();
}

void GameObject::Start()
{
	// 1. 이미 시작 시 return
	if (IsStarted()) return;

	// 2. 계층 구조상 활성화 상태여야 Start 진입 가능
	if (!IsActive()) return;

	// 3. 시작 상태로 전환
	SetGameObjectState(GameObjectState::Started);

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled()) comp->Start();
	}
}

void GameObject::FixedUpdate()
{
	// 1. Active하지 않으면 return
	if (!IsActive()) return;

	// 2. 초기화 및 시작 보장 (Lazy Start)
	EnsureInitialized();

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->FixedUpdate();
	}
}

void GameObject::Update()
{
	if (!IsActive()) return;

	EnsureInitialized();

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->Update();
	}
}

void GameObject::LateUpdate()
{
	if (!IsActive()) return;

	EnsureInitialized();

	for (auto& comp : m_components)
	{
		if (comp->IsEnabled())
			comp->LateUpdate();
	}
}

void GameObject::SetDestroy()
{
	// 1. 이미 죽었으면 무시
	if (IsDead()) return;

	// 2. 상태를 Dead로 전환
	SetGameObjectState(GameObjectState::Dead);
}

void GameObject::OnDestroy()
{
	// 실제 소멸 직전 호출
	for (auto& comp : m_components) comp->OnDestroy();
}

/*===================================//
//   GameObject activation methods   //
//===================================*/
bool GameObject::GetLocalActive() const
{
	// 말 그대로 해당 게임 오브젝트의 active 상태를 반환하는 것이다.
	// 즉, 자기 자신의 active(로컬 active)를 그대로 반환한다.
	return m_active;
}

bool GameObject::IsActive() const
{
	// 1. 나 자신이 꺼져있으면 false
	if (!m_active) return false;

	// 2. 계층 구조 탐색을 위한 트랜스폼 가져오기
	Transform* transform = static_cast<Transform*>(m_componentCache[(usize)ComponentType::Transform]);
	if (!transform) return false;

	// 3. 부모인 경우 true 반환
	if (!transform->GetParent()) return true;

	// 부모의 IsActive() 호출 (재귀)
	// TODO : 이후에는 재귀함수가 아닌 Flat 한 탐색이 이뤄져야 한다.
	return transform->GetParent()->GetOwner()->IsActive();
}

void GameObject::SetActive(bool active)
{
	// 1. 로컬 상태 변화 없으면 리턴
	if (m_active == active) return;

	// 2. 변경 전의 계층 활성 상태 기록
	// (내가 꺼져있었는데 부모도 꺼져있었다면, 내가 켜져도 여전히 꺼진 상태)
	bool wasActive = IsActive();

	// 3. 상태 변경
	m_active = active;

	// 4. 변경 후의 계층 활성 상태 확인
	bool nowActive = IsActive();

	// 5. "실질적인 상태"가 변했을 때만 전파
	// ex) 부모가 꺼져있으면(was=false), 내가 true로 바껴도(now=false) 변화 없음 -> 전파 안 함.
	if (wasActive != nowActive)
		SetActiveState(nowActive);
}

void GameObject::SetActiveState(bool active)
{
	// 1. 내 컴포넌트들에게 알림 (여기서 SetEnable이 호출됨 -> Catch-up 발생!)
	// 주의: SetEnable은 m_enabled 값을 바꾸는 게 아니라, 
	// m_enabled가 true인 놈들에게 OnEnable/OnDisable을 유발시키는 역할을 해야 함.
	// 하지만 Component::SetEnable은 m_enabled를 바꿔버림.
	for (auto& comp : m_components)
	{
		// 컴포넌트 자체가 켜져있을 때만 OnEnable/Disable 반응
		if (comp->IsEnabled())
		{
			if (active)
			{
				// [Catch-up] 여기서도 필요함!
				// 컴포넌트가 켜질 때(OnEnable) 필요한 초기화 보장
				// Component.h에 friend class GameObject가 있으므로 EnsureInitialized 접근 가능
				comp->EnsureInitialized();
				comp->OnEnable();
			}
			else
			{
				comp->OnDisable();
			}
		}
	}

	// 2. 자식 게임 오브젝트들에게도 전파 (재귀)
	// 주의: 자식 자체가 SetActive(false)로 꺼져있다면, 
	// 부모가 켜지든 말든 걔는 계속 꺼져있어야 하므로 전파하면 안 됨.
	// 즉, 자식의 로컬 m_active가 true인 경우에만 계층 변화의 영향을 받음.
	const auto& children = GetTransform().GetChildren();
	for (Transform* childTransform : children)
	{
		GameObject* childGO = childTransform->GetOwner();

		// [중요] 자식의 "로컬 스위치"가 켜져 있을 때만 부모의 영향을 받음.
		// 자식이 꺼져있다면(GetLocalActive() == false), 부모가 켜지든 말든 계속 꺼져있음.
		if (childGO && childGO->GetLocalActive())
		{
			childGO->SetActiveState(active);
		}
	}
}

/*=====================//
//  hierarchy methods  //
//=====================*/
GameObject* GameObject::GetRoot()
{
	return GetTransform().GetRoot();
}

void GameObject::SetParent(GameObject* parent)
{
	// 부모가 바뀌면 계층 활성화 상태가 바뀔 수 있음 (예: 꺼진 부모 -> 켜진 부모)
	bool wasActive = IsActive();
	Transform* parentTransform = (parent != nullptr) ? &parent->GetTransform() : nullptr;
	
	GetTransform().SetParent(parentTransform);

	bool nowActive = IsActive();
	if (wasActive != nowActive) SetActiveState(nowActive);
}

void GameObject::AddChild(GameObject* child)
{
	if (child == nullptr) return;
	child->SetParent(this);
}

/*======================//
//  GameObject helpers  //
//======================*/
void GameObject::EnsureInitialized()
{
	// 컴포넌트에서 사용한 초기화 보장 메서드
	if (!IsAwake())	  Awake();
	if (!IsStarted()) Start();
}