#include "CorePch.h"
#include "Object.h"
#include "ObjectStateMachine.h"

namespace MGF3D
{
	Object::Object() = default;
	Object::~Object() = default;

	Ptr<ObjectStateMachine> Object::GetState() const
	{
		if (!m_stateMachine)
		{
			MGF_LOG_ERROR("Object with ID {} does not have a valid state machine.", m_instanceID.value);
			return nullptr;
		}

		return m_stateMachine.Get();
	}
}