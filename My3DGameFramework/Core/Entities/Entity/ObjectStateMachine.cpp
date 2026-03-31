#include "CorePch.h"
#include "ObjectStateMachine.h"

namespace MGF3D
{
	ObjectStateMachine::ObjectStateMachine(Ptr<Object> owner) : m_owner(owner) { }
	ObjectStateMachine::~ObjectStateMachine() = default;
}