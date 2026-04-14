#pragma once
#include "Entities/Component.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Transform)

	MGF_CLASS_PTR(Camera)
	class Camera : public Component
	{
		MGF_DISABLE_COPY(Camera)
		using Super = Component;

	public:
		Camera(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~Camera() override;
		Camera(Camera&& other) noexcept;
		Camera& operator=(Camera&& other) noexcept;

	/*===============================//
	//       Camera Type             //
	//===============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*===============================//
	//       Camera Methods          //
	//===============================*/
	public:
		// 투영(Projection) 설정
		void SetProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);
		void SetViewportSize(float width, float height);

	public:
		// 핵심 렌더링 행렬 반환
		mat4 GetViewMatrix() const;
		mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
		mat4 GetViewProjectionMatrix() const;

	public:
		// 카메라 파라미터 반환
		float GetNear() const { return m_nearPlane; }
		float GetFar() const { return m_farPlane; }
		float GetFOV() const { return m_fovDegrees; }
		float GetAspectRatio() const { return m_aspectRatio; }

	public:
		bool IsProjectionDirty() const { return m_isProjectionDirty; }
		void SetProjectionDirty() { m_isProjectionDirty = true; }
		void CleanProjectionDirty() { m_isProjectionDirty = false; }

	private:
		Transform* GetTransform() const;

		float m_fovDegrees		{ 45.0f };
		float m_aspectRatio		{ 16.0f / 9.0f };
		float m_nearPlane		{ 0.1f };
		float m_farPlane		{ 100.0f };

		mat4 m_projectionMatrix { 1.0f };
		bool m_isProjectionDirty{ true };
	};
}