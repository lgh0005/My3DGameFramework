#pragma once
#include "Entities/Component.h"

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

		const vec3& GetLocalPosition() const { return m_localPosition; }
		const quat& GetLocalRotation() const { return m_localRotation; }
		const vec3& GetLocalScale() const { return m_localScale; }

		const mat4& GetWorldMatrix() const;
		void SetWorldMatrix(const mat4& matrix) { m_worldMatrix = matrix; }

		bool IsTransformDirty() const { return m_isTransformDirty; }
		void SetTransformDirty() const { m_isTransformDirty = true; }
		void ClearTransformDirty() const { m_isTransformDirty = false; }

	private:

		// 로컬 공간 데이터
		vec3 m_localPosition { 0.0f, 0.0f, 0.0f };
		quat m_localRotation { 1.0f, 0.0f, 0.0f, 0.0f };
		vec3 m_localScale	 { 1.0f, 1.0f, 1.0f };

		// 계산된 월드 행렬
		mutable mat4 m_worldMatrix{ 1.0f };

		// 월드 행렬 갱신 필요 여부
		mutable bool m_isTransformDirty { true };
	};
}