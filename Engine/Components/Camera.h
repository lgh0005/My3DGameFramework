#pragma once
#include "Components/Bases/Component.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(Camera)
class Camera : public Component
{
public:
	static CameraUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Camera;
	virtual ComponentType GetType() const override { return ComponentType::Camera; }

	void SetProjection(float fovDegrees, float aspectRatio, 
					   float nearPlane, float farPlane);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	void LookAt(const glm::vec3& target, 
				const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

	// TODO : 이후에는 GameObject가 들고 있는 Transform을 통해서
	// 접근하여 가져와야 함. 현재는 카메라의 위치/방향을 조작할 때 사용.
	Transform& GetTransform() { return *m_transform; }
	const Transform& GetTransform() const { return *m_transform; }

private:
	Camera() = default;
	bool Init();
	// TODO : 이후에는 GameObject가 들고 있는 Transform을 통해서
	// 접근하여 가져와야 함.
	TransformUPtr m_transform;

	float m_fovDegrees	{ 45.0f };
	float m_aspectRatio	{ (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT};
	float m_nearPlane	{ 0.1f };
	float m_farPlane	{ 100.0f };

	glm::mat4 m_projectionMatrix	{ glm::mat4(1.0f) };
};
