#include "EnginePch.h"
#include "GameObject.h"
#include "Scene/Scene.h"
#include "Object/Component.h"
#include "Components/Transform.h"
#include "Components/Script.h"

DECLARE_DEFAULTS_IMPL(GameObject)

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
	m_instanceID = INVALID_INSTANCE_ID;
	SetName("GameObject");

	// 기본적으로 Transform을 소유
	auto transform = Transform::Create();
	AddComponent(std::move(transform));
	return true;
}

Transform& GameObject::GetTransform() const
{
	return *static_cast<Transform*>(m_componentCache[(usize)ComponentType::Transform]);
}

void GameObject::SetName(const std::string& name)
{
	m_name = name;
	m_nameHash = Utils::StrHash(m_name);
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
	// 1. 나 자신이 꺼져있으면 즉시 false
	if (!m_active) return false;

	// 2. 부모를 타고 올라가며 하나라도 꺼져있는지 검사
	Transform* current = static_cast<Transform*>(m_componentCache[(usize)ComponentType::Transform]);
	if (!current) return false;

	// 3. 부모인 경우 true 반환
	while (current)
	{
		Transform* parent = current->GetParent();
		if (!parent) break; // 부모가 없으면 Root임

		GameObject* parentGO = parent->GetOwner();
		// 부모가 존재하는데 꺼져있다면, 나도 비활성화 상태임
		if (parentGO && !parentGO->GetLocalActive())
			return false;

		// 위로 이동
		current = parent;
	}

	return true;
}

void GameObject::SetActive(bool active)
{
	// 1. 로컬 상태 변화 없으면 리턴
	if (m_active == active) return;

	// 2. 변경 전/후의 "실제 계층 상태" 비교
	bool wasActive = IsActive();
	m_active = active;
	bool nowActive = IsActive();

	// 3. 실질적인 활성화 상태가 변했을 때만 전파
	if (wasActive != nowActive)
		SetActiveState(nowActive);
}

void GameObject::SetActiveState(bool active)
{
	// 1. 작업 목록(Worklist) 생성 - 명시적 스택 역할
	std::vector<GameObject*> workList;
	workList.reserve(16);
	workList.push_back(this);

	// 2. DFS 순회
	while (!workList.empty())
	{
		// 스택에서 하나 꺼냄
		GameObject* currentGO = workList.back();
		workList.pop_back();

		// A. 컴포넌트 처리 (Catch-up & Event)
		for (auto& comp : currentGO->GetComponents())
		{
			if (comp->IsEnabled())
			{
				if (active)
				{
					comp->EnsureInitialized(); // Catch-up
					comp->OnEnable();
				}
				else
				{
					comp->OnDisable();
				}
			}
		}

		// B. 자식들 스택에 추가 (재귀 호출 대체)
		// 주의: 자식의 '로컬 스위치'가 켜져 있는 경우에만 전파 대상임
		const auto& children = currentGO->GetTransform().GetChildren();
		for (Transform* childTransform : children)
		{
			GameObject* childGO = childTransform->GetOwner();
			if (childGO && childGO->GetLocalActive())
				workList.push_back(childGO);
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