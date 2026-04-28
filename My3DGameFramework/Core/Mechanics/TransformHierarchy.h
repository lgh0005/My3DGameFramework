#pragma once
#include "Entities/Mechanic.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Transform)

	MGF_CLASS_PTR(TransformHierarchy)
	class TransformHierarchy : public Mechanic
	{
		MGF_DISABLE_COPY(TransformHierarchy)
		using Super = Mechanic;

	public:
		TransformHierarchy(Component* owner);
		virtual ~TransformHierarchy() override;
		TransformHierarchy(TransformHierarchy&& other) noexcept;
		TransformHierarchy& operator=(TransformHierarchy&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		void SetParent(Transform* parent);
		Transform* GetParent() const;
		const Vector<Transform*>& GetChildren() const;
		void AddChild(Transform* child);
		void RemoveChild(Transform* child);

	public:
		const mat4& GetWorldMatrix() const;
		void SetWorldMatrix(const mat4& matrix);
		bool IsTransformDirty() const { return m_isTransformDirty; }
		void SetTransformDirty() const { m_isTransformDirty = true; }
		void ClearTransformDirty() const { m_isTransformDirty = false; }

	public:
		vec3 ExtractWorldPosition() const;
		quat ExtractWorldRotation() const;
		vec3 ExtractWorldScale() const;

	private:
		Transform* GetTransformOwner() const;
		Transform* m_parent{ nullptr };
		Vector<Transform*> m_children;

		// 월드 상태 데이터
		mutable vec3 m_worldPosition{ 0.0f, 0.0f, 0.0f };
		mutable quat m_worldRotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		mutable vec3 m_worldScale{ 1.0f, 1.0f, 1.0f };

		// 계산된 월드 행렬
		mutable mat4 m_worldMatrix{ 1.0f };

		// 월드 행렬 갱신 필요 여부
		mutable bool m_isTransformDirty{ true };
	};
}