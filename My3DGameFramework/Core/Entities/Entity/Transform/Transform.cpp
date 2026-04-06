#include "CorePch.h"
#include "Transform.h"
#include "Entities/Entity/Transform/TransformStorage.h"
#include "Entities/Entity/GameObject/GameObject.h"
#include "Utils/TransformUtils.h"

namespace MGF3D
{
	Transform::Transform() = default;
	Transform::~Transform() = default;

	/*==================================//
	//          Local Transform         //
	//==================================*/
	void Transform::SetLocalPosition(const vec3& pos)
	{
		m_localPosition = pos;
		m_isLocalTransformDirty = true;
	}

	void Transform::SetLocalRotation(const vec3& eulerDegrees)
	{
		m_localRotation = Rotation(eulerDegrees);
		m_isLocalTransformDirty = true;
	}

	void Transform::SetLocalRotation(const quat& rot)
	{
		m_localRotation = rot;
		m_isLocalTransformDirty = true;
	}

	void Transform::SetLocalScale(const vec3& scale)
	{
		m_localScale = scale;
		m_isLocalTransformDirty = true;
	}

	/*==================================//
	//          World Transform         //
	//==================================*/
	vec3 Transform::GetWorldPosition()
	{
		return TransformUtils::GetTranslation(m_worldMatrix);
	}

	quat Transform::GetWorldRotation()
	{
		return TransformUtils::GetRotation(m_worldMatrix);
	}

	vec3 Transform::GetWorldScale()
	{
		return TransformUtils::GetScale(m_worldMatrix);
	}
}