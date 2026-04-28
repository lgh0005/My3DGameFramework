#include "CorePch.h"
#include "Component.h"
#include "Managers/TypeManager.h"
#include "Managers/EntityManager.h"

namespace MGF3D
{
	Component::Component(ObjectIDHash id, ObjectIDHash ownerID) : Super(id), m_ownerID(ownerID) { }
	Component::~Component() = default;
	Component::Component(Component&& other) noexcept = default;
	Component& Component::operator=(Component&& other) noexcept = default;

	ObjectIDHash Component::GetOwnerID() const
	{
		return m_ownerID;
	}

	void Component::SetEnable(bool enable)
	{
		EObjectLifecycle currentState = GetState();
		EObjectLifecycle targetState = CommonUtils::Select(enable, EObjectLifecycle::Active, EObjectLifecycle::Inactive);
		if (currentState != targetState) SetState(targetState);
	}

	bool Component::IsEnable() const
	{
		// 1. 내 자신의 상태가 켜져 있는지 확인
		EObjectLifecycle currentState = GetState();
		if (currentState != EObjectLifecycle::Active && currentState != EObjectLifecycle::PendingAdd)
			return false;

		// 2. 부모 오브젝트(Owner)의 상태 확인
		if (!m_ownerID.IsValid()) return false;
		GameObject* owner = MGF_ENTITY.GetGameObject(m_ownerID);
		if (!owner) return false;

		EObjectLifecycle ownerState = owner->GetState();
		if (ownerState != EObjectLifecycle::Active && ownerState != EObjectLifecycle::PendingAdd)
			return false;

		return true;
	}

	/*=========================//
	//    Scene Custom Type    //
	//=========================*/
	int16 Component::s_typeIndex = -1;
	const MGFType* Component::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}