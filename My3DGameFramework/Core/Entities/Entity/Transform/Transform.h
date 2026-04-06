#pragma once
#include "Entities/Entity/Transform/TransformDescriptor.h"
#include "Spatial/Position.h"
#include "Spatial/Rotation.h"
#include "Spatial/Scale.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)
	MGF_CLASS_PTR(TransformStorage)

	MGF_CLASS_PTR(Transform)
	class Transform
	{
		friend class TransformStorage;

	public:
		Transform();
		~Transform();
	
	/*==================================//
	//         Local Transform          //
	//==================================*/
	public:

		// Position
		const Position& GetLocalPosition() const { return m_localPosition; }
		void SetLocalPosition(const vec3& pos);

		// Rotation
		const Rotation& GetLocalRotation() const { return m_localRotation; }
		void SetLocalRotation(const vec3& eulerDegrees);
		void SetLocalRotation(const quat& rot);

		// Scale
		const Scale& GetLocalScale() const { return m_localScale; }
		void SetLocalScale(const vec3& scale);

	/*==================================//
	//         World Transform          //
	//==================================*/
	public:
		vec3 GetWorldPosition();
		quat GetWorldRotation();
		vec3 GetWorldScale();

	/*==================================//
	//     Hierarchy Management         //
	//==================================*/
	public:
		usize GetParentIndex() const { return m_parentIndex; }
		const SVector<usize>& GetChildrenIndices() const { return m_childrenIndices; }
		Ptr<GameObject> GetOwner() const { return m_owner; }

	private:

		// owner
		Ptr<GameObject> m_owner { nullptr };

		// Local Data
		Position m_localPosition;
		Rotation m_localRotation;
		Scale    m_localScale;

		// Cached Data
		mat4 m_localMatrix{ 1.0f };
		mat4 m_worldMatrix{ 1.0f };

		// Dirty Flags
		bool m_isLocalTransformDirty{ true };
		bool m_isWorldTransformDirty{ true };

		// Hierarchy (Pure Index)
		usize m_parentIndex	{ static_cast<usize>(-1) };
		SVector<usize> m_childrenIndices;
	};
}