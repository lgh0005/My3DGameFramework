#include "CorePch.h"
#include "Camera.h"
#include "Components/Transform.h"
#include "Managers/TypeManager.h"
#include "Managers/EntityManager.h"

namespace MGF3D
{
	Camera::Camera(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	Camera::~Camera() = default;
	Camera::Camera(Camera&& other) noexcept = default;
	Camera& Camera::operator=(Camera&& other) noexcept = default;

	/*===============================//
	//       Camera Type             //
	//===============================*/
	int16 Camera::s_typeIndex = -1;
	const MGFType* Camera::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	/*===============================//
	//       Camera Methods          //
	//===============================*/
	void Camera::SetProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
	{
		m_fovDegrees = fovDegrees;
		m_aspectRatio = aspectRatio;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;

		m_projectionMatrix = glm::perspective
		(
			Math::ToRadians(m_fovDegrees), 
			m_aspectRatio, 
			m_nearPlane, 
			m_farPlane
		);
		SetProjectionDirty();
	}

	void Camera::SetViewportSize(float width, float height)
	{
		if (width <= 0.0f || height <= 0.0f) return;

		m_aspectRatio = width / height;
		m_projectionMatrix = glm::perspective
		(
			Math::ToRadians(m_fovDegrees),
			m_aspectRatio, 
			m_nearPlane, 
			m_farPlane
		);
		SetProjectionDirty();
	}

	mat4 Camera::GetViewMatrix() const
	{
		Transform* transform = GetTransform();
		if (transform == nullptr) return mat4(1.0f);

		const mat4& world = transform->GetWorldMatrix();

		// 1. 월드 행렬에서 회전 부분(3x3)만 추출하여 전치(Transpose)합니다.
		// 이는 곧 회전의 역행렬과 같습니다.
		mat3 rotation = mat3(world);
		mat3 viewRotation = Math::Transpose(rotation);

		// 2. 월드 행렬에서 위치(Translation) 벡터를 추출합니다.
		vec3 worldPos = vec3(world[3]);

		// 3. 뷰 행렬의 이동 성분을 계산합니다.
		// 카메라의 위치를 반대 방향으로 옮긴 뒤, 회전의 역을 적용합니다.
		vec3 viewTranslation = -(viewRotation * worldPos);

		// 4. 최종 4x4 뷰 행렬 조립 (Column-Major 기준)
		mat4 view = mat4(viewRotation);
		view[3] = vec4(viewTranslation, 1.0f);

		return view;
	}

	mat4 Camera::GetViewProjectionMatrix() const
	{
		return m_projectionMatrix * GetViewMatrix();
	}

	Transform* Camera::GetTransform() const
	{
		return MGF_ENTITY.GetComponent<Transform>(GetOwnerID());
	}
}