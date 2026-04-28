#include "CorePch.h"
#include "Transform.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Transform::Transform(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID), m_hierarchy(this) { }
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

	/*===================================//
	//      Hierarchy & World Methods    //
	//===================================*/
	void Transform::SetParent(Transform* parent)
	{
		m_hierarchy.SetParent(parent);
	}

	Transform* Transform::GetParent() const
	{
		return m_hierarchy.GetParent();
	}

	void Transform::AddChild(Transform* child)
	{
		m_hierarchy.AddChild(child);
	}

	void Transform::RemoveChild(Transform* child)
	{
		m_hierarchy.RemoveChild(child);
	}

	vec3 Transform::GetWorldPosition() const
	{
		return m_hierarchy.ExtractWorldPosition();
	}

	quat Transform::GetWorldRotation() const
	{
		return m_hierarchy.ExtractWorldRotation();
	}

	vec3 Transform::GetWorldScale() const
	{
		return m_hierarchy.ExtractWorldScale();
	}
}