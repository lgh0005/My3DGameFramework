#include "CorePch.h"
#include "Transform.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Transform::Transform(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	Transform::~Transform() = default;
	Transform::Transform(Transform&& other) noexcept = default;
	Transform& Transform::operator=(Transform&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 Transform::s_typeIndex = -1;
	const MGFType* Transform::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void Transform::SetLocalPosition(const vec3& position)
	{
		m_localPosition = position;
		SetTransformDirty();
	}

	void Transform::SetLocalRotation(const vec3& eulerRotation)
	{
		m_localRotation = quat(eulerRotation);
		SetTransformDirty();
	}

	void Transform::SetLocalRotation(const quat& rotation)
	{
		m_localRotation = rotation;
		SetTransformDirty();
	}

	void Transform::SetLocalScale(const vec3& scale)
	{
		m_localScale = scale;
		SetTransformDirty();
	}

	const mat4& Transform::GetWorldMatrix() const
	{
		if (m_isTransformDirty)
		{
			mat4 translation = glm::translate(mat4(1.0f), m_localPosition);
			mat4 rotation = glm::mat4_cast(m_localRotation);
			mat4 scale = glm::scale(mat4(1.0f), m_localScale);
			m_worldMatrix = translation * rotation * scale;
			ClearTransformDirty();
		}

		return m_worldMatrix;
	}
}