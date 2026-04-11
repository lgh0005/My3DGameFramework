#include "EnginePch.h"
#include "Component.h"
#include "Object/GameObject.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(Component)

bool Component::MatchesType(ComponentType type) const
{
	return GetComponentType() == type;
}

GameObject* Component::GetOwner() const
{
	return m_owner;
}

Transform& Component::GetTransform()
{
	// reference transform getter
	return m_owner->GetTransform();
}

const Transform& Component::GetTransform() const
{
	// read-only transform getter
	return m_owner->GetTransform();
}

/*==========================================//
//   default component life-cycle methods   //
//==========================================*/
void Component::Awake() 
{
    // 1. 이미 Awake를 거쳤다면 return
    if (IsAwake()) return;

    // 2. 상태를 Awake로 전환
    SetComponentState(ComponentState::Awake);

    // 3. 사용자 단에서 구현한 OnAwake를 호출
    OnAwake();
}

void Component::Start() 
{
    // 1. 이미 시작 시 return
    if (IsStarted()) return;

    // 2. 컴포넌트가 꺼져 있거나 (m_enable이 꺼짐)
    // 주인도 꺼져있는 경우(주인의 m_active가 꺼짐) return
    if (!IsActive()) return;

    // 3. 시작 상태로 전환
    SetComponentState(ComponentState::Started);

    // 4. 사용자 단에서 구현한 OnStart를 호출
    OnStart();
}

void Component::FixedUpdate() 
{
    if (IsActive())
    {
        EnsureInitialized();
        OnFixedUpdate();
    }
}

void Component::Update() 
{
    if (IsActive())
    {
        EnsureInitialized();
        OnUpdate();
    }
}

void Component::LateUpdate() 
{
    if (IsActive())
    {
        EnsureInitialized();
        OnLateUpdate();
    }
}

void Component::OnDestroy() 
{
    // 1. 이미 죽은 상태라면 중복 호출 방지
    if (IsDead()) return;

    // 2. [Last Will] 죽기 전에 비활성화 처리
    // 만약 컴포넌트가 '동작 중(Active)'이었다면,
    // 죽기 직전에 "나 이제 꺼진다"고 OnDisable을 호출해줘야 짝이 맞음.
    if (IsActive()) OnDisable();

    // 3. 상태를 Dead로 전환 (이제 더 이상 부활 불가)
    SetComponentState(ComponentState::Dead);

    // 4. 사용자 단의 정리 로직 호출 (리소스 해제 등)
    OnDestroyed();
}

/*==========================================//
//   component life-cycle methods by user   //
//==========================================*/
void Component::OnAwake()       { }
void Component::OnStart()       { }
void Component::OnFixedUpdate() { }
void Component::OnUpdate()      { }
void Component::OnLateUpdate()  { }
void Component::OnDestroyed()   { }

/*==================================//
//   component activation methods   //
//==================================*/
void Component::SetEnable(bool enable)
{
    // 0. 상태 변화 없으면 리턴 (가장 먼저 거름)
    if (m_enabled == enable) return;

    // 1. 일단 내 스위치 상태 변경
    m_enabled = enable;

    // 2. 판단을 위해 "주인이 활동 중인가?"를 미리 캡처
    bool isOwnerActive = IsOwnerActive();

    // 3. 총 4가지의 경우가 존재하는데 (m_enable과 isOwnerActive)
    // 이 4가지의 경우 중 판별해야 하는 상황은 다음의 두 가지이다.
    // i) 이 컴포넌트가 켜져있는 상태 && 주인도 켜져있는 상태 : OnEnable 호출
    // ii) 이 컴포넌트를 끌 때 && 주인은 켜져있는 상태 : OnDisable 호출
    // 그 외의 경우에는 어차피 컴포넌트를 호출할 수가 없다.

    // 3-1. [CASE 1] 나를 켠다 (true) && 주인도 켜져있다 (Active)
    if (enable && isOwnerActive)
    {
        // 3-1-1. 컴포넌트의 초기화 보장
        EnsureInitialized();

        // 3-1-2. 이제 안전하게 활성화 이벤트 실행
        OnEnable();
    }

    // 3-2. [CASE 2] 나를 끈다 (false) && 주인은 켜져있다 (Active)
    else if (!enable && isOwnerActive) OnDisable();
}
void Component::OnEnable() {}
void Component::OnDisable() {}

/*===========================================================//
//   owner and self(this component) state checking methods   //
//===========================================================*/
bool Component::IsEnabled() const
{
    return m_enabled;
}

bool Component::IsOwnerActive() const
{
    return m_owner && m_owner->IsActive();
}

bool Component::IsActive() const
{
    return IsEnabled() && IsOwnerActive();
}

/*===================================//
//   component state checking API    //
//===================================*/
void Component::EnsureInitialized()
{
    // 1. 상태가 Awake가 안되어 있다면 즉시 Awake부터 실행
    if (!IsAwake()) Awake();

    // 2. Awake는 지났는데 시작을 안했다면 즉시 Start부터 실행
    if (!IsStarted()) Start();
}