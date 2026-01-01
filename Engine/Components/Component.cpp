#include "EnginePch.h"
#include "Component.h"
#include "Scene/GameObject.h"
#include "Components/Transform.h"

Component::Component() = default;
Component::~Component() = default;

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
	// read only transform getter
	return m_owner->GetTransform();
}

void Component::SetEnable(bool enable)
{
    if (m_enabled == enable) return;

    m_enabled = enable;

    // "내가 켜졌는데 주인도 켜져있는 상태라면" 이벤트를 쏜다
    if (IsActive())
    {
        if (m_enabled) OnEnable();
        else OnDisable();
    }
}

bool Component::IsEnabled() const
{
    return m_enabled;
}

bool Component::IsActive() const
{
    // 1. 나 자신이 켜져 있고 (IsEnabled)
    // 2. 주인(GameObject)이 존재하며
    // 3. 주인도 계층구조상 켜져 있어야 함 (IsActiveInHierarchy)
    return m_enabled && m_owner && m_owner->IsActiveInHierarchy();
}

/*========================================//
//   component life-cycle state methods   //
//========================================*/
void Component::Awake()		  { }
void Component::OnEnable()	  { }
void Component::Start()       { }

void Component::FixedUpdate() { }
void Component::Update()	  { }
void Component::LateUpdate()  { }

void Component::OnDisable()   { }
void Component::OnDestroy()   { }