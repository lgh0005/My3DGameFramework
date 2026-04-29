#pragma once
#include "Entities/Component.h"
#include "Mechanics/TransformHierarchy.h"
#include "Mechanics/TransformCoordinate.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Transform)
	class Transform : public Component
	{
		MGF_DISABLE_COPY(Transform)
		using Super = Component;

	public:
		Transform(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~Transform() override;
		Transform(Transform&& other) noexcept;
		Transform& operator=(Transform&& other) noexcept;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*===================================//
	//      default transform methods    //
	//===================================*/
	public:
		void SetLocalPosition(const vec3& position);
		void SetLocalRotation(const vec3& eulerRotation);
		void SetLocalRotation(const quat& quatRotation);
		void SetLocalScale(const vec3& scale);

		vec3 GetForward() const;
		vec3 GetRight()   const;
		vec3 GetUp()      const;

		const vec3& GetLocalPosition() const { return m_localPosition; }
		const quat& GetLocalRotation() const { return m_localRotation; }
		const vec3& GetLocalScale() const { return m_localScale; }

		const mat4& GetWorldMatrix() const { return m_hierarchy.GetWorldMatrix(); }
		bool IsTransformDirty() const { return m_hierarchy.IsTransformDirty(); }
		void SetTransformDirty() const { m_hierarchy.SetTransformDirty(); }

	/*=====================================//
	//      default translation methods    //
	//=====================================*/
	public:
		void Translate(const vec3& delta);
		void Rotate(const vec3& eulerDelta);
		void Scale(const vec3& delta);

	/*===================================//
	//      Hierarchy & World Methods    //
	//===================================*/
	public:
		void SetParent(Transform* parent);
		Transform* GetParent() const;
		void AddChild(Transform* child);
		void RemoveChild(Transform* child);

		vec3 GetWorldPosition() const;
		quat GetWorldRotation() const;
		vec3 GetWorldScale() const;

	private:

		// 계층 및 좌표계 부품 (Mechanic)
		TransformHierarchy m_hierarchy;
		TransformCoordinate m_coordinate;

		// 로컬 공간 데이터
		vec3 m_localPosition { 0.0f, 0.0f, 0.0f };
		quat m_localRotation { 1.0f, 0.0f, 0.0f, 0.0f };
		vec3 m_localScale	 { 1.0f, 1.0f, 1.0f };
	};
}