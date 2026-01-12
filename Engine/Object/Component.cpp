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
    return m_enabled && m_owner && m_owner->IsActiveInHierarchy();
}

/*========================================//
//   component life-cycle state methods   //
//========================================*/
void Component::Awake()		  { }
void Component::Start()       { }

void Component::FixedUpdate() { }
void Component::Update()	  { }
void Component::LateUpdate()  { }

void Component::OnEnable()    { }
void Component::OnDisable()   { }
void Component::OnDestroy()   { }