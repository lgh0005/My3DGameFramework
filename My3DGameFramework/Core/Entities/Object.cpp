#include "CorePch.h"
#include "Object.h"

namespace MGF3D
{
	Object::Object(ObjectIDHash id)
	{
		m_info.id = id;
		m_info.lifecycle.SetState(EObjectLifecycle::PendingAdd);
	}
	Object::~Object() = default;

	Object::Object(Object&&) noexcept = default;
	Object& Object::operator=(Object&&) noexcept = default;

	ObjectIDHash Object::GetID() const
	{
		return m_info.id;
	}

	EObjectLifecycle Object::GetState() const
	{
		return m_info.lifecycle.GetState();
	}

	void Object::SetState(EObjectLifecycle newState)
	{
		m_info.lifecycle.SetState(newState);
	}
}
